#pragma once

#include "ICompressor.h"

namespace quantpressor
{
	namespace compressors
	{
		class HuffmanCompressor : public quantpressor::ICompressor
		{
		private:
			struct ColumnInfo
			{
				double bps;
				double dx;
				double min, max, avg;
			};

			ColumnInfo compress_column( module_api::uint column,
										const module_api::pIGrid &grid,
										const quantpressor::Quantization &quantization,
										const quantpressor::IBinaryOutputStream &stream );

		public:
			virtual quantpressor::CompressionResult compress( const module_api::pIGrid &grid,
															  const quantpressor::Quantizations &quantizations,
															  const quantpressor::IBinaryOutputStream &stream ) override;

			virtual module_api::pIGrid decompress( const quantpressor::IBinaryInputStream &stream ) override;
		};
	}
}
