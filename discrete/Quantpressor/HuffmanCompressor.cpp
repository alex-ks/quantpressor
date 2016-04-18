#include "HuffmanCompressor.h"
#include <HuffmanCompressor.h>

namespace Quantpressor
{
	namespace Compressors
	{
		HuffmanCompressor::HuffmanCompressor( )
			: NativeCompressorWrapper( new quantpressor::compressors::HuffmanCompressor( ) )
		{
			
		}
	}
}