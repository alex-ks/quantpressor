#include "FastEmpiricalDistribution.h"

#include <exception.h>
#include <random>
#include <cmath>

namespace distributions
{
	unsigned int FastEmpiricalDistribution::MAX_SAMPLE_COUNT = 1e3;

	FastEmpiricalDistribution::FastEmpiricalDistribution( const module_api::pIGrid &grid,
														  module_api::uint column,
														  const DetailedApproximationMethod &method ) : INF( std::numeric_limits<double>::max( ) / 1e3 ), density_sample( 0 )
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

		density_func = [_h, _m, grid, step, count, column, kernel]( double x )
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

		moment_2 = [count, step, grid, column, h, _h, _m, kernel_integral, kernel_moment_1, kernel_moment_2]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count * step; i += step )
			{
				auto value = grid->get_value( i, column );
				double ta = ( a - value ) * _h, tb = ( b - value ) * _h;

				result += h * h * kernel_moment_2( ta, tb );

				result += 2.0 * h * value * kernel_moment_1( ta, tb );

				result += value * value * kernel_integral( ta, tb );
			}

			return result * _m;
		};
	}

	FastEmpiricalDistribution::FastEmpiricalDistribution( std::vector<double> &&sample,
														  const DetailedApproximationMethod & method ) : INF( std::numeric_limits<double>::max( ) / 1e3 ), density_sample( std::move( sample ) )
	{
		auto count = density_sample.size( );
		auto data = density_sample.data( );

		double h = method.window;
		double _h = 1.0 / ( method.window );
		double _m = 1.0 / count;

		auto &kernel = method.kernel;

		density_func = [_h, _m, data, count, kernel]( double x )
		{
			double result = 0.0;

			for ( int i = 0; i < count; ++i )
			{
				result += kernel( ( x - data[i] ) * _h );
			}

			result *= _h * _m;

			return result;
		};

		auto &kernel_integral = method.kernel_integral;

		density_integral = [_h, _m, data, count, kernel_integral]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count; ++i )
			{
				auto value = data[i];

				result += kernel_integral( ( a - value ) * _h,
										   ( b - value ) * _h );
			}

			return result * _m;
		};

		auto &kernel_moment_1 = method.kernel_moment_1;

		moment_1 = [count, data, h, _h, _m, kernel_integral, kernel_moment_1]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count; ++i )
			{
				auto value = data[i];

				result += h * kernel_moment_1( ( a - value ) * _h,
											   ( b - value ) * _h );

				result += value * kernel_integral( ( a - value ) * _h,
												   ( b - value ) * _h );
			}

			return result * _m;
		};

		auto &kernel_moment_2 = method.kernel_moment_2;

		moment_2 = [count, data, h, _h, _m, kernel_integral, kernel_moment_1, kernel_moment_2]( double a, double b )
		{
			double result = 0.0;

			for ( int i = 0; i < count; ++i )
			{
				auto value = data[i];
				double ta = ( a - value ) * _h, tb = ( b - value ) * _h;

				result += h * h * kernel_moment_2( ta, tb );

				result += 2.0 * h * value * kernel_moment_1( ta, tb );

				result += value * value * kernel_integral( ta, tb );
			}

			return result * _m;
		};
	}

	double FastEmpiricalDistribution::density( double x ) const
	{
		return density_func( x );
	}

	double FastEmpiricalDistribution::operator()( double x ) const
	{
		return density_integral( -INF, x );
	}

	double FastEmpiricalDistribution::expectation( ) const
	{
		return moment_1( -INF, INF );
	}

	double FastEmpiricalDistribution::expectation( double a, double b ) const
	{
		return moment_1( a, b ) / density_integral( a, b );
	}

	double FastEmpiricalDistribution::deviation( ) const
	{
		double ex = expectation( );
		return moment_2( -INF, INF ) - ex * ex;
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

	DetailedApproximationMethod DetailedApproximationMethod::gauss_kernel( double window )
	{
		double k = 1.0 / sqrt( 2 * std::_Pi );
		const double _sq2 = 1.0 / sqrt( 2.0 );
		auto gauss_kernel = [k]( double r ) { return k * exp( -0.5 * r * r ); };
		DetailedApproximationMethod method;
		method.kernel = gauss_kernel;
		method.kernel_integral = [_sq2]( double a, double b )
		{
			return 0.5 * ( std::erf( b * _sq2 ) - std::erf( a * _sq2 ) );
		};
		method.kernel_moment_1 = [k]( double a, double b )
		{
			return -k * ( exp( -0.5 * b * b ) - exp( -0.5 * a * a ) );
		};

		auto m2_func = [k, _sq2]( double x ) { return 0.5 * std::erf( x * _sq2 ) - k * x * exp( -0.5 * x * x ); };

		method.kernel_moment_2 = [m2_func]( double a, double b )
		{
			return m2_func( b ) - m2_func( a );
		};
		method.window = window;

		return std::move( method );
	}
}
