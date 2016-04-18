#pragma once
#include "NativeCompressorWrapper.h"

namespace Quantpressor
{
	namespace Compressors
	{
		public ref class LZ77HuffmanCompressor : public NativeCompressorWrapper
		{
		public:
			LZ77HuffmanCompressor( int windowWidth );
		};
	}
}
