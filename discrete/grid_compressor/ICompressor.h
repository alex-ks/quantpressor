#pragma once
#include "Quantizer.h"
#include "IBinaryInputStream.h"
#include "IBinaryOutputStream.h"

#include <IO.h>
#include <vector>
#include <map>

namespace quantpressor
{
	typedef std::vector<Quantization> Quantizations;

	struct CompressionResult
	{
		std::vector<double> columns_bps;
		std::vector<double> real_variances;
		std::vector<double> min_errors, avg_errors, max_errors;
		std::map<std::wstring, std::wstring> extra_results;
	};

	class ICompressor
	{
	public:
		///<summary>
		///<para>Compresses specified grid to stream.</para>
		///<para>Returns structure describing compression characteristics.</para>
		///</summary>
		virtual CompressionResult compress( const module_api::pIGrid &grid,
											const Quantizations &quantizations,
											IBinaryOutputStream &stream ) const = 0;

		virtual module_api::pIGrid decompress( IBinaryInputStream &stream ) const = 0;

		virtual ~ICompressor( ) { }
	};
}