#include "ExponentialDistribution.h"

#include <cmath>

#include <exception.h>

grid_generator::ExponentialDistribution::ExponentialDistribution( double lambda ) : 
	distr( lambda ),
	lambda( lambda )
{
	if ( lambda < 1e-7 )
		throw module_api::BadInputException( L"Lambda must be positive" );
}

double grid_generator::ExponentialDistribution::operator()( double x ) const
{
	if ( x < 0.0 )
		return 0.0;
	return 1.0 - std::exp( -lambda * x );
}

double grid_generator::ExponentialDistribution::expectation( ) const
{
	return 1.0 / lambda;
}

double grid_generator::ExponentialDistribution::expectation( double a, double b ) const
{
	if ( a < 1e-7 )
		a = 0.0;
	if ( b < 1e-7 )
		b = 0.0;
	auto func = [&]( double x ) { return x > -1e-7 ? ( std::exp( -lambda * x ) * ( -1.0 / lambda - x ) ) : 0; };
	return ( func( b ) - func( a ) ) / ( this->operator()( b ) - this->operator()( a ) );
}

double grid_generator::ExponentialDistribution::generate( )
{
	return distr( random_source );
}
