#pragma once
#include "Quantpressor.h"
#include <ICompressor.h>

namespace Quantpressor
{
	namespace Compressors
	{
		public ref class NativeCompressorWrapper : public ICompressor
		{
		private:
			quantpressor::ICompressor *nativeCompressor;

		internal:
			NativeCompressorWrapper( quantpressor::ICompressor *native );

		public:
			!NativeCompressorWrapper( );

			virtual ICompressionResult ^Compress( IGrid ^grid,
												  System::Collections::Generic::IList<IQuantization ^> ^quantizations,
												  IBinaryOutputStream ^stream );

			virtual IGrid ^Decompress( IBinaryInputStream ^stream );
		};
	}
}
