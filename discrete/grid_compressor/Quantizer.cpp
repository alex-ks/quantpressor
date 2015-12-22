#include "Quantizer.h"
#include <random>
#include <utility>
#include <cmath>

grid_compressor::Quantization::Quantization( int quant_count ) :
	borders( quant_count ),
	codes( quant_count - 1 ),
	deviation( 0.0 ),
	entropy( 0.0 )
{

}

grid_compressor::Quantizer::Quantizer( double left, double right )
{
	L = left;
	R = right;
}

grid_compressor::Quantization grid_compressor::Quantizer::quantize( int quant_count, double max_error, const grid_generator::IDistribution & distribution ) const
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
		for ( int i = 1; i < quant_count - 1; ++i )
			q.borders[i] = ( q.codes[i] + q.codes[i - 1] ) / 2;

		invariant_count = 0;

		for ( int i = 0; i < quant_count - 1; ++i )
		{
			auto c = distribution.expectation( q.borders[i], q.borders[i + 1] );

			if ( std::abs( q.codes[i] - c ) < 1e-7 )
				invariant_count++;

			q.codes[i] = c;
		}

		eps = 0.0;

		for ( int i = 0; i < quant_count - 1; ++i )
			eps += distribution.deviation( q.borders[i], q.borders[i + 1] );
	}
	while ( eps > max_error && invariant_count < quant_count - 1 );

	q.deviation = eps;

	for ( int i = 1; i < quant_count; ++i )
	{
		double p = distribution( q.borders[i] ) - distribution( q.borders[i - 1] );
		q.entropy -= p * std::log2( p );
	}

	return std::move( q );
}
