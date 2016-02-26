#pragma once
#include "Quantpressor.h"

#include <Quantizer.h>

namespace Quantpressor
{
	public ref class Quantizer
	{
	private:
		quantpressor::Quantizer *quantizer;

	public:
		Quantizer( double left, double right );
		!Quantizer( );

		IQuantization ^Quantize( int quantCount,
								 double max_error,
								 IDistribution ^distribution );
	};
}
