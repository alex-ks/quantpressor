#include "EmpiricalDistribution.h"

#include "TrapezoidalIntegrator.h"

#include <exception.h>

#include <cmath>

using namespace integrators;

namespace distributions
{
	unsigned int EmpiricalDistribution::MAX_SAMPLE_COUNT = 1e3;

	EmpiricalDistribution::EmpiricalDistribution( const module_api::pIGrid &grid,
												  module_api::uint column,
												  const ApproximationMethod &method,
												  double min_value,
												  double max_value,
												  double error_order ) : 
		min( min_value ),
		max( max_value ),
		error( error_order )
	{
		ex_cache = std::nan( "" );
		dx_cache = std::nan( "" );

		auto count = grid->get_row_count( );
		unsigned int step = 1;

		if ( count > MAX_SAMPLE_COUNT )
		{
			step = count / MAX_SAMPLE_COUNT;
			count = MAX_SAMPLE_COUNT;
		}

		double _h = 1.0 / ( method.window );
		double _m = 1.0 / count;

		density = [=]( double x )
		{
			double result = 0.0;

//#pragma omp parallel for reduction( + : result )
			for ( int i = 0; i < count * step; i += step )
			{
				result += method.kernel( ( x - grid->get_value( i, column ) ) * _h );
			}

			result *= _h * _m;

			return result;
		};
	}

	double EmpiricalDistribution::integrate( const integrators::RealFunction &func, double left, double right ) const
	{
		//assume that max density value is 1.0
		double step = error * 2.0 / ( right - left );

		auto integrator = TrapezoidalIntegrator( step );

		return integrator.integrate( func, left, right );
	}

	double EmpiricalDistribution::operator()( double x ) const
	{
		if ( x <= min )
			return 0.0;
		if ( x > max )
			return 1.0;

		return integrate( density, min, x );
	}

	double EmpiricalDistribution::expectation( ) const
	{
		if ( std::isnan( ex_cache ) )
		{
			auto func = [&]( double t ) { return t * density( t ); };
			double &ex = const_cast<double &>( ex_cache );
			ex = integrate( func, min, max );
		}

		return ex_cache;
	}

	double EmpiricalDistribution::expectation( double a, double b ) const
	{
		if ( a > b )
			std::swap( a, b );

		a = a > min ? a : min;
		b = b < max ? b : max;

		auto func = [&]( double t ) { return t * density( t ); };

		return integrate( func, a, b ) / integrate( density, a, b );
	}

	double EmpiricalDistribution::deviation( ) const
	{
		if ( std::isnan( dx_cache ) )
		{
			double &dx = const_cast<double &>( dx_cache );

			auto moment2 = [&]( double t ) { return t * t * density( t ); };
			auto moment1 = [&]( double t ) { return t * density( t ); };

			double ex = integrate( moment1, min, max );

			dx = integrate( moment2, min, max ) - ex * ex;
		}

		return dx_cache;
	}

	double EmpiricalDistribution::deviation( double a, double b ) const
	{
		if ( a > b )
			std::swap( a, b );

		a = a > min ? a : min;
		b = b < max ? b : max;

		auto moment2 = [&]( double t ) { return t * t * density( t ); };

		double ex = expectation( a, b );
		double prob = this->operator()( b ) - this->operator()( a );
		double m2 = integrate( moment2, a, b );

		return m2 - ex * ex * prob;
	}

	double EmpiricalDistribution::generate( )
	{
		throw module_api::NotImplementedException( );
	}
}
