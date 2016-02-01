#pragma once

#include <lib.h>
#include <vector>

namespace grid_compressor
{
	struct Quantization
	{
		std::vector<double> borders, codes;
		double deviation;
		double entropy;

		Quantization( int quant_count );
	};

	class Quantizer
	{
	private:
		double L, R;

	public:
		Quantizer( double left, double right );

		Quantization quantize( int quant_count, double max_error, const distributions::IDistribution &distribution ) const;
	};
}