#include "LZ77HuffmanCompressor.h"
#include <LZ77HuffmanCompressor.h>

namespace Quantpressor
{
	namespace Compressors
	{
		LZ77HuffmanCompressor::LZ77HuffmanCompressor( int windowWidth )
			: NativeCompressorWrapper( new quantpressor::compressors::LZ77HuffmanCompressor( windowWidth ) )
		{
			
		}
	}
}
