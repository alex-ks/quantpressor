#pragma once
#include <vector>
#include "IInitialValuesConstructor.h"

namespace quantpressor
{
	class SampleBasedConstructor : public IInitialValuesConstructor
	{
	private:
		std::vector<double> sample;

	public:
		SampleBasedConstructor( const std::vector<double> sample );
		~SampleBasedConstructor( ) override;

		Quantization construct_initial_values( int quant_count ) const override;
	};
}
