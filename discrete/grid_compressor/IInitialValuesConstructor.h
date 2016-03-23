#pragma once
#include <vector>

namespace quantpressor
{
	struct Quantization
	{
		std::vector<double> borders, codes;
		double deviation;
		double entropy;

		Quantization( int quant_count ) :
			borders( quant_count ),
			codes( quant_count - 1 ),
			deviation( 0.0 ),
			entropy( 0.0 )
		{ }

		Quantization( )
		{ }

		int get_quant_count( ) const;
	};

	class IInitialValuesConstructor
	{
	public:
		virtual Quantization construct_initial_values( int quant_count ) const = 0;

		virtual ~IInitialValuesConstructor( )
		{

		}
	};
}
