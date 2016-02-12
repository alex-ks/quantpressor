#pragma once

#include "ICompressor.h"

namespace quantpressor
{
	namespace arithmetic_coding
	{
		typedef unsigned int BlockIndexType;

		class ModularArithmeticCompressor : public quantpressor::ICompressor
		{
		private:
			static const BlockIndexType DEFAULT_BLOCK_SIZE = 16;

			BlockIndexType block_size;

			double compress_column( module_api::uint column,
									const module_api::pIGrid &grid,
									const quantpressor::Quantization &quantization,
									const quantpressor::IBinaryOutputStream &stream );

		public:
			ModularArithmeticCompressor( );
			ModularArithmeticCompressor( BlockIndexType block_size );
			~ModularArithmeticCompressor( ) override;

			quantpressor::CompressionResult compress( const module_api::pIGrid &grid,
													  const quantpressor::Quantizations &quantizations,
													  const quantpressor::IBinaryOutputStream &stream ) override;

			module_api::pIGrid decompress( const quantpressor::IBinaryInputStream &stream ) override;
		};
	}
}