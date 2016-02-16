#include "LZ77HuffmanCompressor.h"
#include "utilities.h"
#include "huffman.h"
#include <exception.h>


namespace quantpressor
{
	namespace compressors
	{
		LZ77HuffmanCompressor::LZ77HuffmanCompressor( size_t window_width ) : width( window_width )
		{
			
		}

		CompressionResult LZ77HuffmanCompressor::compress( const module_api::pIGrid & grid,
														   const Quantizations & quantizations,
														   const IBinaryOutputStream & stream )
		{
			Window<int> codes_window( width );
			Window<int> lengths_window( width );

			throw module_api::NotImplementedException( );
		}

		module_api::pIGrid LZ77HuffmanCompressor::decompress( const IBinaryInputStream & stream )
		{
			return module_api::pIGrid( );
		}
	}
}
