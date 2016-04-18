#include "ArithmeticCodingCompressor.h"
#include <ArithmeticCodingCompressor.h>

namespace Quantpressor
{
	namespace Compressors
	{
		ArithmeticCodingCompressor::ArithmeticCodingCompressor( ) 
			: NativeCompressorWrapper( new quantpressor::compressors::ArithmeticCodingCompressor( ) )
		{

		}
	}
}
