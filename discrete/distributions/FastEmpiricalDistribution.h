#pragma once

#include <io.h>
#include <functional>

#include "IDistribution.h"

namespace distributions
{
	typedef std::function<double( double )> RealFunction;
	typedef std::function<double( double, double )> DefiniteIntegral;

	struct DetailedApproximationMethod
	{
		RealFunction kernel;
		DefiniteIntegral kernel_integral;
		DefiniteIntegral kernel_moment_1;
		DefiniteIntegral kernel_moment_2;
		double window;

		static DetailedApproximationMethod gauss_kernel( double window );
		static DetailedApproximationMethod gauss_kernel( const std::vector<double> &sample );
	};

	class FastEmpiricalDistribution : public IDistribution
	{
	private:
		RealFunction density_func;
		DefiniteIntegral density_integral, moment_1, moment_2;
		
		std::vector<double> density_sample;
		
		const double INF;

	public:
		static unsigned int MAX_SAMPLE_COUNT;

		FastEmpiricalDistribution( const module_api::pIGrid &grid,
								   module_api::uint column,
								   const DetailedApproximationMethod &method );

		FastEmpiricalDistribution( std::vector<double> &&sample,
								   const DetailedApproximationMethod &method );

		double density( double x ) const override;
		double operator()( double x ) const override;
		double expectation( ) const override;
		double expectation( double a, double b ) const override;
		double deviation( ) const override;
		double deviation( double a, double b ) const override;
		double generate( ) override;

		~FastEmpiricalDistribution( ) override;
	};
}
