#include "Quantizer.h"

#include "NativeQuantization.h"
#include "INativeDistribution.h"

namespace Quantpressor
{
	Quantizer::Quantizer( double left, double right )
	{
		quantizer = new grid_compressor::Quantizer( left, right );
	}

	Quantizer::!Quantizer( )
	{
		if ( quantizer != nullptr )
		{
			delete quantizer;
			quantizer = nullptr;
		}
	}

	IQuantization ^Quantizer::Quantize( int quantCount,
										double maxError,
										IDistribution ^distribution )
	{
		auto wrapper = dynamic_cast< INativeDistribution ^ >( distribution );
		if ( wrapper == nullptr )
			throw gcnew System::ArgumentException( "Can handle only native distributions" );

		auto &nativeDistr = *( wrapper->NativePtr( ) );
		return gcnew NativeQuantization( quantizer->quantize( quantCount, maxError, nativeDistr ) );
	}
}