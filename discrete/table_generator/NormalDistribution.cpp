#include "NormalDistribution.h"

#include <exception.h>
#include <cmath>

grid_generator::NormalDistribution::NormalDistribution( double mean, double deviation ) :
	distr( mean, deviation ),
	alpha( mean ),
	sigma( deviation ),
	root_factor( 1.0 / ( std::sqrt( 2.0 ) * deviation ) )
{
	if ( deviation < 1e-7 )
		throw module_api::BadInputException( L"Deviation must be positive" );
}

double grid_generator::NormalDistribution::operator()( double x ) const
{
	auto func = [&]( double x ) { return -0.5 * std::erf( ( alpha - x ) * root_factor ); };
	return func( x ) - func( -std::numeric_limits<double>::infinity( ) );
}

double grid_generator::NormalDistribution::expectation( ) const
{
	return alpha;
}

double grid_generator::NormalDistribution::expectation( double a, double b ) const
{
	auto func = [&]( double x )
	{
		return -0.5 * alpha * std::erf( ( alpha - x ) * root_factor ) 
			- sigma * std::exp( -( alpha - x ) * ( alpha - x ) / 2.0 / ( sigma * sigma ) ) / sqrt( 2.0 * std::_Pi );
	};
	return ( func( b ) - func( a ) ) / ( this->operator()( b ) - this->operator()( a ) );
}

double grid_generator::NormalDistribution::generate( )
{
	return distr( random_source );
}

double grid_generator::NormalDistribution::deviation( ) const
{
	return sigma * sigma;
}

double grid_generator::NormalDistribution::deviation( double a, double b ) const
{
	auto moment2 = [&]( double x )
	{
		return 0.5 * ( alpha * alpha + sigma * sigma ) * std::erf( ( x - alpha ) * root_factor )
			- sigma * ( alpha + x ) * std::exp( -( alpha - x ) * ( alpha - x ) / 2.0 / ( sigma * sigma ) ) / sqrt( 2.0 * std::_Pi );
	};

	auto ex = expectation( a, b );

	return moment2( b ) - moment2( a ) - ex * ex * ( this->operator()( b ) - this->operator()( a ) );
}
