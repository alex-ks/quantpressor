#include "Quantizer.h"
#include <random>
#include <utility>
#include <cmath>

quantpressor::Quantization::Quantization( int quant_count ) :
	borders( quant_count ),
	codes( quant_count - 1 ),
	deviation( 0.0 ),
	entropy( 0.0 )
{

}

quantpressor::Quantizer::Quantizer( double left, double right )
{
	L = left;
	R = right;
}

quantpressor::Quantization quantpressor::Quantizer::quantize( int quant_count, double max_error, const distributions::IDistribution & distribution ) const
{
	Quantization q( quant_count );

	double delta = ( R - L ) / quant_count;

	q.borders[0] = L;
	q.borders[quant_count - 1] = R;
	q.codes[0] = L + delta;

	for ( int i = 1; i < q.codes.size( ); ++i )
		q.codes[i] = q.codes[i - 1] + delta;

	double eps;
	int invariant_count;

	do
	{
		{		
#pragma omp parallel for
			for ( int i = 1; i < quant_count - 1; ++i )
				q.borders[i] = ( q.codes[i] + q.codes[i - 1] ) / 2;

			invariant_count = 0;

#pragma omp parallel for
			for ( int i = 0; i < quant_count - 1; ++i )
			{
				auto c = distribution.expectation( q.borders[i], q.borders[i + 1] );

				if ( std::abs( q.codes[i] - c ) < 1e-7 )
					invariant_count++;

				q.codes[i] = c;
			}

			eps = 0.0;

#pragma omp parallel for reduction( + : eps )
			for ( int i = 0; i < quant_count - 1; ++i )
				eps += distribution.deviation( q.borders[i], q.borders[i + 1] );
		}
	}
	while ( eps > max_error && invariant_count < quant_count - 1 );

	q.deviation = eps;

	double entropy = 0.0;

#pragma omp parallel for reduction( - : entropy )
	for ( int i = 1; i < quant_count; ++i )
	{
		double p = distribution( q.borders[i] ) - distribution( q.borders[i - 1] );
		entropy -= p * std::log2( p );
	}

	q.entropy = entropy;

	return std::move( q );
}
