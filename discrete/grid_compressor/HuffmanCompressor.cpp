#include "HuffmanCompressor.h"
#include "huffman.h"

#include <RoughGrid.h>

#include <exception.h>
#include <utility>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <iostream>

#include "utilities.h"

namespace quantpressor
{
	namespace compressors
	{
		using namespace quantpressor;
		using namespace huffman;

		using module_api::uint;
		using module_api::pIGrid;
		using igor::RoughGrid;

		using std::map;
		using std::vector;
		using std::string;
		using std::priority_queue;

		HuffmanCompressor::ColumnInfo HuffmanCompressor::compress_column( uint column,
																		  const pIGrid &grid,
																		  const Quantization &quantization,
																		  IBinaryOutputStream &s ) const
		{
			ColumnInfo result = ColumnInfo( );
			result.min = std::numeric_limits<double>::max( );
			result.max = std::numeric_limits<double>::min( );
			result.avg = 0;
			result.dx = 0;
			result.bps = 0;

			map<double, ull> counts;
			auto &stream = const_cast< IBinaryOutputStream & >( s );

			for ( double code : quantization.codes )
			{
				counts[code] = 0UL;
			}

			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				++counts[find_code( grid->get_value( i, column ), quantization )];
			}

			HuffmanTree<double> tree( counts );

			ull bit_count = 0UL;

			for ( auto &pair : counts )
			{
				if ( pair.second != 0UL )
				{
					bit_count += pair.second * tree.get_encoding_length( pair.first );
				}
			}

			tree.serialize( stream );

			stream << bit_count;

			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				double value = grid->get_value( i, column );
				double code = find_code( value, quantization );
				bit_set coding = tree.get_encoding( code );

				for ( int j = 0; j < coding.size( ); ++j )
				{
					stream.write_bit( coding[j] );
				}

				auto error = fabs( value - code );
				auto square_error = pow( value - code, 2 );

				result.max = result.max > error ? result.max : error;
				result.min = result.min < error ? result.min : error;
				result.avg = i > 0 ? ( result.avg * i + error ) / ( i + 1 ) : error;

				result.dx = i > 0 ? ( result.dx * i + square_error ) / ( i + 1 ) : square_error;
			}

			result.bps = static_cast<double>( bit_count ) / grid->get_row_count( );

			return std::move( result );
		}

		CompressionResult HuffmanCompressor::compress( const pIGrid &grid,
													   const Quantizations &quantizations,
													   IBinaryOutputStream &stream ) const
		{
			stream << grid->get_row_count( );
			stream << grid->get_column_count( );

			if ( grid->get_column_count( ) != quantizations.size( ) )
			{
				throw module_api::BadInputException( L"Quantizations count must be equal to column count" );
			}

			CompressionResult result = CompressionResult( );

			for ( uint i = 0; i < grid->get_column_count( ); ++i )
			{
				auto info = compress_column( i, grid, quantizations[i], stream );
				result.columns_bps.push_back( info.bps );
				result.real_variances.push_back( info.dx );
				result.min_errors.push_back( info.min );
				result.avg_errors.push_back( info.avg );
				result.max_errors.push_back( info.max );
			}

			return std::move( result );
		}

		module_api::pIGrid HuffmanCompressor::decompress( quantpressor::IBinaryInputStream &stream ) const
		{
			unsigned int row_count, column_count;
			stream >> row_count;
			stream >> column_count;

			auto grid = module_api::make_heap_aware<RoughGrid>( row_count, column_count );

			for ( int column = 0; column < column_count; ++column )
			{
				auto tree = HuffmanTree<double>::deserialize( stream );

				ull bit_count;
				stream >> bit_count;

				uint row = 0;

				for ( ull i = 0; i < bit_count; ++i )
				{
					auto value = tree.make_step( stream.read_bit( ) );

					if ( value != nullptr )
					{
						grid->set_value( row++, column, *value );
					}
				}
			}

			return grid;
		}
	}
}
