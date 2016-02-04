#include "FastEmpiricalDistribution.h"

#include <exception.h>

namespace distributions
{
	unsigned int FastEmpiricalDistribution::MAX_SAMPLE_COUNT = 1e3;

	using std::numeric_limits;

	FastEmpiricalDistribution::FastEmpiricalDistribution( const module_api::pIGrid &grid,
														  module_api::uint column,
														  const DetailedApproximationMethod &method )
	{
		auto count = grid->get_row_count( );
		unsigned int step = 1;

		if ( count > MAX_SAMPLE_COUNT )
		{
			step = count / MAX_SAMPLE_COUNT;
			count = MAX_SAMPLE_COUNT;
		}

		double h = method.window;
		double _h = 1.0 / ( method.window );
		double _m = 1.0 / count;

		auto &kernel = method.kernel;

		density = [_h, _m, grid, step, count, column, kernel]( double x )
		{
			double result = 0.0;

			for ( int i = 0; i < count * step; i += step )
			{
				result += kernel( ( x - grid->get_value( i, column ) ) * _h );
			}

			result *= _h * _m;

			return result;
		};

		auto &kernel_integral = method.kernel_integral;

		density_integral = [_h, _m, grid, count, step, column, kernel_integral]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count * step; i += step )
			{
				auto value = grid->get_value( i, column );

				result += kernel_integral( ( a - value ) * _h,
										   ( b - value ) * _h );
			}

			return result * _m;
		};

		auto &kernel_moment_1 = method.kernel_moment_1;

		moment_1 = [count, step, grid, column, h, _h, _m, kernel_integral, kernel_moment_1]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count * step; i += step )
			{
				auto value = grid->get_value( i, column );

				result += h * kernel_moment_1( ( a - value ) * _h,
											   ( b - value ) * _h );

				result += value * kernel_integral( ( a - value ) * _h,
												   ( b - value ) * _h );
			}

			return result * _m;
		};

		auto &kernel_moment_2 = method.kernel_moment_2;

		moment_2 = [=]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count * step; i += step )
			{
				auto value = grid->get_value( i, column );

				result += h * h * kernel_moment_2( ( a - value ) * _h,
												   ( b - value ) * _h );

				result += 2.0 * h * value * kernel_moment_1( ( a - value ) * _h,
															 ( b - value ) * _h );

				result = value * value * kernel_integral( ( a - value ) * _h,
														  ( b - value ) * _h );
			}

			return result * _m;
		};
	}

	double FastEmpiricalDistribution::operator()( double x ) const 
	{
		return density_integral( -numeric_limits<double>::infinity( ), x );
	}

	double FastEmpiricalDistribution::expectation( ) const
	{
		auto inf = numeric_limits<double>::infinity( );
		return moment_1( -inf, inf );
	}

	double FastEmpiricalDistribution::expectation( double a, double b ) const
	{
		return moment_1( a, b ) / density_integral( a, b );
	}

	double FastEmpiricalDistribution::deviation( ) const
	{
		throw module_api::NotImplementedException( );
	}

	double FastEmpiricalDistribution::deviation( double a, double b ) const
	{
		if ( a > b )
			std::swap( a, b );

		double ex = expectation( a, b );

		return moment_2( a, b ) - ex * ex * density_integral( a, b );
	}

	double FastEmpiricalDistribution::generate( )
	{
		throw module_api::NotImplementedException( );
	}

	FastEmpiricalDistribution::~FastEmpiricalDistribution( ) { }
}
