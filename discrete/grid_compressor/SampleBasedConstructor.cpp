#include "SampleBasedConstructor.h"

#include <algorithm>

namespace quantpressor
{
	SampleBasedConstructor::SampleBasedConstructor( double left, double right, const std::vector<double> sample )
	{
		this->sample = sample;
		std::sort( this->sample.begin( ), this->sample.end( ) );
		L = left;
		R = right;
	}

	SampleBasedConstructor::~SampleBasedConstructor( )
	{
	
	}

	Quantization quantpressor::SampleBasedConstructor::construct_initial_values( int quant_count ) const
	{
		Quantization q( quant_count );

		q.borders[0] = L;
		q.borders[quant_count - 1] = R;

		q.codes[0] = sample[0];
		q.codes[quant_count - 2] = sample[sample.size( ) - 1];
		auto step = ( double )( sample.size( ) - 1 ) / ( quant_count - 2 );

		for ( int i = 1; i < quant_count - 2; ++i )
		{
			auto x = i * step;
			auto t = ( int )x;
			q.codes[i] = ( sample[t + 1] - sample[t] ) * ( x - t ) + sample[t];
		}

		return std::move( q );
	}
}
