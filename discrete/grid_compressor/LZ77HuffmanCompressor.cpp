#include "LZ77HuffmanCompressor.h"
#include "utilities.h"
#include "huffman.h"
#include <exception.h>
#include <RoughGrid.h>

namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
		using std::map;
		using module_api::uint;
		using module_api::make_heap_aware;
		using huffman::HuffmanTree;
		using huffman::DynamicHuffmanTree;
		using igor::RoughGrid;

		struct ColumnInfo
		{
			double bps;
			double dx;
			double min, max, avg;
		};

		LZ77HuffmanCompressor::LZ77HuffmanCompressor( width_t window_width ) : width( window_width )
		{
			
		}

		HuffmanTree<double> create_column_tree( const module_api::pIGrid &grid, 
												uint column, 
												const Quantization &q, 
												ColumnInfo *result )
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
														   IBinaryOutputStream & stream ) const
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

			auto writer = TripletWriter( stream, width );

			width_t max_len = -1;
			double avg_len = 0;
			index_t triplet_count = 0;

			index_t i;
			for ( i = 0; i < symbol_count; ++i )
			{
				width_t dist = 0, length = 0;
				index_t start = window_start + ( i % column_count + column_count - window_start % column_count ) % column_count;

				for ( index_t j = start; j < i; )
				{
					width_t curr_length = 0;
					while ( find_code_index( grid->get_value( ( i + curr_length ) / column_count, 
															  ( i + curr_length ) % column_count ), 
											 quantizations[( i + curr_length ) % column_count] )
							== find_code_index( grid->get_value( j / column_count, 
																 j % column_count ), 
												quantizations[j % column_count] )
							&& i + curr_length < symbol_count
							&& j < i )
					{
						++curr_length;
						++j;
					}

					if ( curr_length > length )
					{
						length = curr_length;
						dist = j - curr_length - window_start;
					}

					++j;
					j += ( i % column_count + column_count - j % column_count ) % column_count;
				}

				auto &code = symbol_trees[( i + length ) % column_count].get_encoding( find_code( grid->get_value( ( i + length ) / column_count, ( i + length ) % column_count ), 
																								  quantizations[( i + length ) % column_count] ) );
				writer.write_triplet( dist, length, code );	

				i += length;

				max_len = max_len > length ? max_len : length;
				avg_len = triplet_count != 0 ? ( avg_len * triplet_count + length ) / ( triplet_count + 1 ) : length;
				++triplet_count;

				if ( i - window_start > width )
				{
					window_start += i - window_start - width;
				}
			}

			if ( i == symbol_count )
			{
				auto &code = symbol_trees[i % column_count].get_encoding( find_code( grid->get_value( i / column_count, i % column_count ), 
																					 quantizations[i % column_count] ) );
				writer.write_triplet( 0, 0, code );
			}

			writer.flush( );

			auto bps = double( stream.get_current_position( ) - start_pos ) / ( symbol_count + 1 );

			for ( int i = 0; i < column_count; ++i )
				result.columns_bps.push_back( bps );

			result.extra_results[L"Maximal sequence length"] = std::to_wstring( max_len );
			result.extra_results[L"Average sequence length"] = std::to_wstring( avg_len );

			return result;
		}

		module_api::pIGrid LZ77HuffmanCompressor::decompress( IBinaryInputStream &stream ) const
		{
			uint column_count, row_count;
			stream >> row_count >> column_count;

			vector<HuffmanTree<double>> symbol_trees;
			for ( uint i = 0; i < column_count; ++i )
			{
				symbol_trees.push_back( HuffmanTree<double>::deserialize( stream ) );
			}

			auto reader = TripletReader( stream, width, std::move( symbol_trees ) );
			auto result = make_heap_aware<RoughGrid>( row_count, column_count );
			vector<width_t> dist_cache, length_cache;

			auto symbol_count = index_t( column_count ) * row_count;

			index_t window_start = 0;

			for ( index_t i = 0; i < symbol_count; ++i )
			{
				width_t distance, length;
				double value;
				reader.read_triplet( &distance, &length, &value );

				for ( index_t j = window_start + distance; j < window_start + distance + length; ++j, ++i )
				{
					result->set_value( i / column_count, i % column_count,
									   result->get_value( j / column_count, j % column_count ) );
				}

				result->set_value( i / column_count, i % column_count, value );

				if ( i - window_start > width )
				{
					window_start += i - window_start - width;
				}
			}

			return result;
		}
	}
}
