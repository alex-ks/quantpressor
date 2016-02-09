#include "NormalDistribution.h"

#include <exception.h>
#include <cmath>

distributions::NormalDistribution::NormalDistribution( double mean, double deviation ) :
	distr( mean, deviation ),
	alpha( mean ),
	sigma( deviation ),
	root_factor( 1.0 / ( std::sqrt( 2.0 ) * deviation ) )
{
	if ( deviation < 1e-7 )
		throw module_api::BadInputException( L"Deviation must be positive" );
}

double distributions::NormalDistribution::density( double x ) const
{
	return std::exp( -( alpha - x ) * ( alpha - x ) / 2.0 / ( sigma * sigma ) ) / sqrt( 2.0 * std::_Pi );
}

double distributions::NormalDistribution::operator()( double x ) const
{
	auto func = [&]( double t ) { return -0.5 * std::erf( ( alpha - t ) * root_factor ); };
	return func( x ) - func( -std::numeric_limits<double>::infinity( ) );
}

double distributions::NormalDistribution::expectation( ) const
{
	return alpha;
}

double distributions::NormalDistribution::expectation( double a, double b ) const
{
	auto func = [&]( double t )
	{
		return -0.5 * alpha * std::erf( ( alpha - t ) * root_factor ) 
			- sigma * std::exp( -( alpha - t ) * ( alpha - t ) / 2.0 / ( sigma * sigma ) ) / sqrt( 2.0 * std::_Pi );
	};
	return ( func( b ) - func( a ) ) / ( this->operator()( b ) - this->operator()( a ) );
}

double distributions::NormalDistribution::generate( )
{
	return distr( random_source );
}

double distributions::NormalDistribution::deviation( ) const
{
	return sigma * sigma;
}

double distributions::NormalDistribution::deviation( double a, double b ) const
{
	auto moment2 = [&]( double t )
	{
		return 0.5 * ( alpha * alpha + sigma * sigma ) * std::erf( ( t - alpha ) * root_factor )
			- sigma * ( alpha + t ) * std::exp( -( alpha - t ) * ( alpha - t ) / 2.0 / ( sigma * sigma ) ) / sqrt( 2.0 * std::_Pi );
	};

	auto ex = expectation( a, b );

	return moment2( b ) - moment2( a ) - ex * ex * ( this->operator()( b ) - this->operator()( a ) );
}
