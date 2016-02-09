#pragma once

#include "ICompressor.h"

namespace huffman
{
	class HuffmanCompressor : public grid_compressor::ICompressor
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
									const grid_compressor::Quantization &quantization,
									const grid_compressor::IBinaryOutputStream &stream );

	public:
		virtual grid_compressor::CompressionResult compress( const module_api::pIGrid &grid,
															 const grid_compressor::Quantizations &quantizations,
															 const grid_compressor::IBinaryOutputStream &stream ) override;

		virtual module_api::pIGrid decompress( const grid_compressor::IBinaryInputStream &stream ) override;
	};
}
