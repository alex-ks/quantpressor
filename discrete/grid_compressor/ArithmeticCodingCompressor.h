#pragma once

#include "ICompressor.h"

#include <vector>
#include <unordered_map>

namespace quantpressor
{
	namespace compressors
	{

		class ArithmeticCodingCompressor : public ICompressor
		{
		private:
			void serialize_frequences( const std::vector<std::unordered_map<double, ull>> &frequences,
									   IBinaryOutputStream &stream ) const;

			std::vector<std::unordered_map<double, ull>> deserialize_cumulative( module_api::uint column_count,
																				 IBinaryInputStream &stream ) const;

		public:
			typedef ull register_type;

			CompressionResult compress( const module_api::pIGrid &grid, 
										const Quantizations &quantizations, 
										IBinaryOutputStream &stream ) const override;

			module_api::pIGrid decompress( IBinaryInputStream &stream ) const override;
		};
	}
}
