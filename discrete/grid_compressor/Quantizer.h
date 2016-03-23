#pragma once

#include <lib.h>
#include <vector>

#include "IInitialValuesConstructor.h"

namespace quantpressor
{
	class UniformConstructor : public IInitialValuesConstructor
	{
	private:
		double L, R;

	public:
		Quantization construct_initial_values( int quant_count ) const override;

		UniformConstructor( double left, double right );
		~UniformConstructor( ) override;
	};

	class Quantizer
	{
	private:
		IInitialValuesConstructor *constructor;

		static const int START_N;
		static const int SEARCH_ITER_COUNT;
		static const double ERROR_AREA;

	public:
		Quantizer( double left, double right );
		Quantizer( IInitialValuesConstructor *constructor );
		~Quantizer( );

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