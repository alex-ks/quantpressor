#include "SampleBasedConstructor.h"

#include <algorithm>

namespace quantpressor
{
	SampleBasedConstructor::SampleBasedConstructor( const std::vector<double> sample )
	{
		this->sample = sample;
		std::sort( this->sample.begin( ), this->sample.end( ) );
	}

	SampleBasedConstructor::~SampleBasedConstructor( )
	{
	
	}

	Quantization quantpressor::SampleBasedConstructor::construct_initial_values( int quant_count ) const
	{
		Quantization q;

		if ( sample.size( ) > quant_count - 1 )
		{
			// todo: implement
		}
		else
		{
			// todo: implement
		}

		return std::move( q );
	}
}
