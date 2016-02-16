#include "LZ77HuffmanCompressor.h"
#include "utilities.h"
#include "huffman.h"
#include <exception.h>


namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
		using module_api::uint;

		LZ77HuffmanCompressor::LZ77HuffmanCompressor( size_t window_width ) : width( window_width )
		{
			
		}

		CompressionResult LZ77HuffmanCompressor::compress( const module_api::pIGrid & grid,
														   const Quantizations & quantizations,
														   const IBinaryOutputStream & stream )
		{
			//Window<int> window( width );
			vector<int> cached_dists,
				cached_lengths,
				cached_codes_idexes;
			
			auto column_count = grid->get_column_count( );
			auto row_count = grid->get_row_count( );
			uint window_start = 0U;

			for ( uint i = 0; i < row_count; ++i )
			{
				for ( uint j = 0; j < column_count; ++j )
				{
					int code = find_code_index( grid->get_value( i, j ), quantizations[i] );
					uint dist = 0, max_len = 0;

					for ( uint k = window_start; k < i * column_count + j; ++k )
					{
						if ( find_code_index( grid->get_value( k / column_count, k % column_count ), quantizations[k % column_count] ) == code )
						{
							uint len = 1U;

							while ( ( k + len < i * column_count + j ) &&
									( i + ( j + len ) / column_count ) < row_count * column_count &&
									find_code_index( 
										grid->get_value( ( k + len ) / column_count, ( k + len ) % column_count ),
										quantizations[( k + len ) % column_count] ) 
									==
									find_code_index( 
										grid->get_value( i + ( j + len ) / column_count, ( j + len ) % column_count ),
										quantizations[( j + len ) % column_count] ) )
							{
								++len;
							}
							--len;

							if ( max_len < len )
							{
								max_len = len;
								dist = k;
							}
						}
					}
					//todo: get mismatch symbol and archive triple, replace double 'for' by single
				}
			}

			throw module_api::NotImplementedException( );
		}

		module_api::pIGrid LZ77HuffmanCompressor::decompress( const IBinaryInputStream & stream )
		{
			throw module_api::NotImplementedException( );
		}
	}
}
