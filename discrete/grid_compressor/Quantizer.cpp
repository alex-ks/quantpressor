#include "Quantizer.h"
#include <random>
#include <utility>
#include <cmath>
#include <atomic>

const int quantpressor::Quantizer::START_N = 100;
const int quantpressor::Quantizer::SEARCH_ITER_COUNT = 10;
const double quantpressor::Quantizer::ERROR_AREA = 0.1;

using std::sqrt;
using std::fabs;

quantpressor::Quantization::Quantization( int quant_count ) :
	borders( quant_count ),
	codes( quant_count - 1 ),
	deviation( 0.0 ),
	entropy( 0.0 )
{

}

quantpressor::Quantization::Quantization( )
{

}

int quantpressor::Quantization::get_quant_count( ) const
{
	return borders.size( );
}

quantpressor::Quantizer::Quantizer( double left, double right )
{
	L = left;
	R = right;
}

quantpressor::Quantization quantpressor::Quantizer::quantize( int quant_count, 
															  double max_error, 
															  const distributions::IDistribution & distribution ) const
{
	return quantize( quant_count, -1, max_error, distribution );
}

quantpressor::Quantization quantpressor::Quantizer::quantize( int quant_count, 
															  int iteration_count, 
															  double max_error, 
															  const distributions::IDistribution & distribution ) const
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
	int count = 0;

	do
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
			{
#pragma omp atomic
				++invariant_count;
			}

			q.codes[i] = c;
		}

		eps = 0.0;

//#pragma omp parallel for reduction( + : eps )
		for ( int i = 0; i < quant_count - 1; ++i )
			eps += distribution.deviation( q.borders[i], q.borders[i + 1] );
		++count;
	}
	while ( eps > max_error && invariant_count < quant_count - 1 && ( iteration_count <= 0 || count < iteration_count ) );

	q.deviation = eps;

	double entropy = 0.0;

#pragma omp parallel for reduction( - : entropy )
	for ( int i = 1; i < quant_count; ++i )
	{
		double p = distribution( q.borders[i] ) - distribution( q.borders[i - 1] );
		entropy -= p != 0.0 ? p * std::log2( p ) : 0.0;
	}

	q.entropy = entropy;

	return std::move( q );
}

quantpressor::Quantization quantpressor::Quantizer::quantize( double max_error, 
															  const distributions::IDistribution & distribution ) const
{
	Quantization q;
	auto n = START_N;
	int left = 2; //quantization must have at least one range with two borders

	q = quantize( START_N, SEARCH_ITER_COUNT, max_error, distribution );

	while ( q.deviation > max_error )
	{
		left = n;
		n *= 2;
		q = quantize( n, SEARCH_ITER_COUNT, max_error, distribution );
	}

	auto right = n;
	n = left + ( right - left ) / 2;

	while ( fabs( q.deviation - max_error ) > ERROR_AREA * max_error
			&& n != left
			&& n != right )
	{
		q = quantize( n, SEARCH_ITER_COUNT, max_error, distribution );

		if ( q.deviation > max_error )
		{ left = n; }
		else
		{ right = n; }
		n = left + ( right - left ) / 2;
	}
	
	q = quantize( q.get_quant_count( ), max_error, distribution );

	return std::move( q );
}
