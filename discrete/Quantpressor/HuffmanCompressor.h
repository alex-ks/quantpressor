#pragma once
#include "Quantpressor.h"

#include <HuffmanCompressor.h>

namespace Quantpressor
{
	namespace Compressors
	{
		public ref class HuffmanCompressor : public ICompressor
		{
		private:
			huffman::HuffmanCompressor *nativeCompressor;

		public:
			HuffmanCompressor( );
			!HuffmanCompressor( );

			virtual ICompressionResult ^Compress( IGrid ^grid,
												  System::Collections::Generic::IList<IQuantization ^> ^quantizations,
												  IBinaryOutputStream ^stream );

			virtual IGrid ^Decompress( IBinaryInputStream ^stream );
		};
	}
}