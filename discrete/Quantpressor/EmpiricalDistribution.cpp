#include "EmpiricalDistribution.h"
#include "NativeGrid.h"

#include <vector>

using namespace Quantpressor;
using namespace Quantpressor::Distributions;

const int DEFAULT_SAMPLE_COUNT = 1e3;

#pragma unmanaged

#include <random>

distributions::FastEmpiricalDistribution *CreateNativeDisribution( std::vector<double> &&sample )
{
	distributions::DetailedApproximationMethod method;

	double k = 1.0 / sqrt( 2 * std::_Pi );
	const double _sq2 = 1.0 / sqrt( 2.0 );

	method.kernel = [k]( double r ) { return k * exp( -0.5 * r * r ); };
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

	method.window = 0.35;
	
	return new distributions::FastEmpiricalDistribution( std::move( sample ), method );
}

distributions::FastEmpiricalDistribution *CreateNativeDisribution( const module_api::pIGrid &grid, module_api::uint column )
{
	distributions::DetailedApproximationMethod method;

	double k = 1.0 / sqrt( 2 * std::_Pi );
	const double _sq2 = 1.0 / sqrt( 2.0 );

	method.kernel = [k]( double r ) { return k * exp( -0.5 * r * r ); };
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

	method.window = 0.35;

	return new distributions::FastEmpiricalDistribution( grid, column, method );
}

void DeleteNativeDistribution( distributions::FastEmpiricalDistribution *distr )
{
	delete distr;
}

#pragma managed

EmpiricalDistribution::EmpiricalDistribution( IGrid ^grid, int column, int maxSampleCount )
{
	auto count = grid->RowCount;
	int step = 1;

	if ( count > maxSampleCount )
	{
		step = count / maxSampleCount;
		count = maxSampleCount;
	}

	std::vector<double> sample( count );

	for ( int i = 0; i < count; ++i )
	{
		sample[i] = grid->GetValue( i * step, column );
	}

	nativeDistribution = CreateNativeDisribution( std::move( sample ) );

	/*auto wrapper = dynamic_cast< NativeGrid ^ > ( grid );

	if ( wrapper != nullptr )
	{
		module_api::pIGrid nativeGrid = wrapper->NativePtr( );
		nativeDistribution = CreateNativeDisribution( nativeGrid, column );
	}
	else
	{
		std::vector<double> sample( count );

		for ( int i = 0; i < count; ++i )
		{
			sample[i] = grid->GetValue( i * step, column );
		}

		nativeDistribution = CreateNativeDisribution( std::move( sample ) );
	}*/
}

EmpiricalDistribution::EmpiricalDistribution( IGrid ^grid, int column ) : EmpiricalDistribution( grid, column, DEFAULT_SAMPLE_COUNT )
{
	
}

EmpiricalDistribution::!EmpiricalDistribution( )
{
	DeleteNativeDistribution( nativeDistribution );
	nativeDistribution = nullptr;
}

double EmpiricalDistribution::Density( double x )
{
	return nativeDistribution->density( x );
}

double EmpiricalDistribution::Cumulative( double x )
{
	return ( *nativeDistribution )( x );
}

double EmpiricalDistribution::TotalExpectation::get( )
{
	return nativeDistribution->expectation( );
}

double EmpiricalDistribution::Expectation( double a, double b )
{
	return nativeDistribution->expectation( a, b );
}

double EmpiricalDistribution::TotalVariance::get( )
{
	return nativeDistribution->deviation( );
}

double EmpiricalDistribution::Variance( double a, double b )
{
	return nativeDistribution->deviation( a, b );
}

distributions::IDistribution * Quantpressor::Distributions::EmpiricalDistribution::NativePtr( )
{
	return nativeDistribution;
}
