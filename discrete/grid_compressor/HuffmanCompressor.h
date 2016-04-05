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
										quantpressor::IBinaryOutputStream &stream ) const;

		public:
			virtual CompressionResult compress( const module_api::pIGrid &grid,
												const quantpressor::Quantizations &quantizations,
												quantpressor::IBinaryOutputStream &stream ) const override;

			virtual module_api::pIGrid decompress( quantpressor::IBinaryInputStream &stream ) const override;
		};
	}
}
