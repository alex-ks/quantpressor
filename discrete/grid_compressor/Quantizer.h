#pragma once

#include <lib.h>
#include <vector>

namespace quantpressor
{
	struct Quantization
	{
		std::vector<double> borders, codes;
		double deviation;
		double entropy;

		Quantization( int quant_count );
		Quantization( );

		int get_quant_count( ) const;
	};

	class Quantizer
	{
	private:
		double L, R;

		static const int START_N;
		static const int SEARCH_ITER_COUNT;
		static const double ERROR_AREA;

	public:
		Quantizer( double left, double right );

		Quantization quantize( int quant_count, 
							   double max_error, 
							   const distributions::IDistribution &distribution ) const;

		Quantization quantize( int quant_count,
							   int iteration_count,
							   double max_error,
							   const distributions::IDistribution &distribution ) const;

		Quantization quantize( double max_error,
							   const distributions::IDistribution &distribution ) const;
	};
}