#pragma once
#include "Quantizer.h"

namespace grid_compressor
{
	typedef unsigned long long ull;

	inline double find_code( double value, const Quantization &quantization )
	{
		module_api::uint left = 0, right = quantization.codes.size( );
		module_api::uint i = ( right - left ) / 2;

		while ( !( value > quantization.borders[i] && value <= quantization.borders[i + 1] ) )
		{
			if ( value > quantization.borders[i] )
			{
				left = i + 1;
			}
			else
			{
				right = i;
			}
			i = left + ( right - left ) / 2;
		}

		return quantization.codes[i];
	}
}
