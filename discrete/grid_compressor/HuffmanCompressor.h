#pragma once

#include "ICompressor.h"

namespace huffman
{
	class HuffmanCompressor : public grid_compressor::ICompressor
	{
	private:
		double compress_column( module_api::uint column,
								const module_api::pIGrid &grid,
								const grid_compressor::Quantization &quantization,
								const grid_compressor::IBinaryOutputStream &stream );

	public:
		virtual std::vector<double> compress( const module_api::pIGrid &grid,
											  const grid_compressor::Quantizations &quantizations,
											  const grid_compressor::IBinaryOutputStream &stream ) override;

		virtual module_api::pIGrid decompress( const grid_compressor::IBinaryInputStream &stream ) override;
	};
}
