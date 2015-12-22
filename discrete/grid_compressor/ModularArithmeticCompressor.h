#pragma once

#include "ICompressor.h"

namespace arithmetic_coding
{
	typedef unsigned int BlockIndexType;

	class ModularArithmeticCompressor : public grid_compressor::ICompressor
	{
	private:
		static const BlockIndexType DEFAULT_BLOCK_SIZE = 16;

		BlockIndexType block_size;

		double compress_column( module_api::uint column,
								const module_api::pIGrid &grid,
								const grid_compressor::Quantization &quantization,
								const grid_compressor::IBinaryOutputStream &stream );

	public:
		ModularArithmeticCompressor( );
		ModularArithmeticCompressor( BlockIndexType block_size );
		~ModularArithmeticCompressor( ) override;

		std::vector<double> compress( const module_api::pIGrid &grid,
									  const grid_compressor::Quantizations &quantizations,
									  const grid_compressor::IBinaryOutputStream &stream ) override;

		module_api::pIGrid decompress( const grid_compressor::IBinaryInputStream &stream ) override;
	};
}