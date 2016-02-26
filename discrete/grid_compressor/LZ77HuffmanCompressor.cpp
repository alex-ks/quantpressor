#include "LZ77HuffmanCompressor.h"
#include "utilities.h"
#include "huffman.h"
#include <exception.h>

namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
		using std::map;
		using module_api::uint;
		using huffman::HuffmanTree;
		using huffman::DynamicHuffmanTree;

		struct ColumnInfo
		{
			double bps;
			double dx;
			double min, max, avg;
		};

		LZ77HuffmanCompressor::LZ77HuffmanCompressor( width_t window_width ) : width( window_width )
		{
			
		}

		HuffmanTree<double> create_column_tree( const module_api::pIGrid &grid, uint column, const Quantization &q, ColumnInfo *result )
		{
			map<double, ull> counts;

			for ( double code : q.codes )
			{
				counts[code] = 0UL;
			}

			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				double value = grid->get_value( i, column );
				double code = find_code( value, q );
				++counts[code];

				auto error = fabs( value - code );
				auto square_error = pow( value - code, 2 );

				result->max = result->max > error ? result->max : error;
				result->min = result->min < error ? result->min : error;
				result->avg = i > 0 ? ( result->avg * i + error ) / ( i + 1 ) : error;

				result->dx = i > 0 ? ( result->dx * i + square_error ) / ( i + 1 ) : square_error;
			}

			return HuffmanTree<double>( counts );
		}

		CompressionResult LZ77HuffmanCompressor::compress( const module_api::pIGrid & grid,
														   const Quantizations & quantizations,
														   IBinaryOutputStream & stream )
		{
			auto column_count = grid->get_column_count( );
			auto row_count = grid->get_row_count( );
			index_t window_start = 0;

			stream << row_count << column_count;

			vector<HuffmanTree<double>> symbol_trees;

			CompressionResult result;

			for ( uint i = 0; i < column_count; ++i )
			{
				ColumnInfo info;
				symbol_trees.push_back( create_column_tree( grid, i, quantizations[i], &info ) );
				symbol_trees[i].serialize( stream );

				result.real_variances.push_back( info.dx );
				result.min_errors.push_back( info.min );
				result.avg_errors.push_back( info.avg );
				result.max_errors.push_back( info.max );
			}

			auto start_pos = stream.get_current_position( );

			auto symbol_count = index_t( row_count ) * column_count - 1;

			auto writer = TripletWriter( stream );

			for ( index_t i = 0; i < symbol_count; ++i )
			{
				width_t dist = 0, length = 0;
				size_t curr_column = i % column_count;
				index_t start = window_start + ( curr_column + column_count - window_start % column_count ) % column_count;

				for ( index_t j = start; j < i; j += column_count )
				{
					width_t curr_length = 0;
					while ( find_code_index( grid->get_value( ( i + curr_length ) / column_count, ( i + curr_length ) % column_count ), quantizations[( i + curr_length ) % column_count] )
							== find_code_index( grid->get_value( ( j + curr_length ) / column_count, ( j + curr_length ) % column_count ), quantizations[( j + curr_length ) % column_count] )
							&& i + curr_length < symbol_count
							&& j + curr_length < i )
					{
						++curr_length;
					}

					if ( curr_length > length )
					{
						length = curr_length;
						dist = j - window_start;
					}
				}

				auto &code = symbol_trees[( i + length ) % column_count].get_encoding( find_code( grid->get_value( ( i + length ) / column_count, ( i + length ) % column_count ), quantizations[( i + length ) % column_count] ) );
				writer.write_triplet( dist, length, code );

				i += length;

				if ( i - window_start > width )
				{
					window_start += i - window_start - width;
				}

				curr_column = ++curr_column % column_count;
			}

			auto bps = double( stream.get_current_position( ) - start_pos ) / ( symbol_count + 1 );

			for ( int i = 0; i < column_count; ++i )
				result.columns_bps[i] = bps;

			return result;
		}

		module_api::pIGrid LZ77HuffmanCompressor::decompress( IBinaryInputStream &stream )
		{
			throw module_api::NotImplementedException( );
		}
	}
}
