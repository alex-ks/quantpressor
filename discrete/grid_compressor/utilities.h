#pragma once
#include "Quantizer.h"

namespace quantpressor
{
	typedef unsigned long long ull;

	inline unsigned int find_code_index( double value, const Quantization &quantization )
	{
		module_api::uint left = 0, right = quantization.codes.size( );
		module_api::uint i = ( right - left ) / 2;

		while ( !( value > quantization.borders[i] && value <= quantization.borders[i + 1] )
				&& left != right )
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

		return i;
	}

	inline double find_code( double value, const Quantization &quantization )
	{
		auto i = find_code_index( value, quantization );
		return quantization.codes[i];
	}
}
