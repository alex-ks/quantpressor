#pragma once
#include "Quantizer.h"
#include "IBinaryInputStream.h"
#include "IBinaryOutputStream.h"

#include <IO.h>
#include <vector>

namespace grid_compressor
{
	typedef std::vector<Quantization> Quantizations;

	class ICompressor
	{
	public:

		///<summary>
		///<para>Compresses specified grid to stream</para>
		///<para>Returns vector of avetage bit counts per symbol for each column</para>
		///</summary>
		virtual std::vector<double> compress( const module_api::pIGrid &grid,
											  const Quantizations &quantizations,
											  const IBinaryOutputStream &stream ) = 0;

		virtual module_api::pIGrid decompress( const IBinaryInputStream &stream ) = 0;

		virtual ~ICompressor( ) { }
	};
}