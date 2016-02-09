#pragma once
#include "Quantpressor.h"

#include <Quantizer.h>

namespace Quantpressor
{
	ref class NativeQuantization : public IQuantization
	{
	private:
		grid_compressor::Quantization *nativeQuantization;

	internal:
		grid_compressor::Quantization NativeStruct( );

	public:
		NativeQuantization( grid_compressor::Quantization &&quantization );
		!NativeQuantization( );

		virtual property array<double>^ Borders 
		{
			array<double> ^get( );
		}

		virtual property array<double>^ Codes
		{
			array<double> ^get( );
		}

		virtual property Double Variance
		{
			double get( );
		}

		virtual property Double Entropy
		{
			double get( );
		}
	};
}
