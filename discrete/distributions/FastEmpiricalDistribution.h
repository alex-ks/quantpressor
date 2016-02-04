#pragma once

#include <io.h>
#include <functional>

#include "IDistribution.h"

namespace distributions
{
	typedef std::function<double( double )> RealFunction;
	typedef std::function<double( double, double )> DefinteIntegral;

	struct DetailedApproximationMethod
	{
		RealFunction kernel;
		DefinteIntegral kernel_integral;
		DefinteIntegral kernel_moment_1;
		DefinteIntegral kernel_moment_2;
		double window;
	};

	class FastEmpiricalDistribution : public IDistribution
	{
	private:
		RealFunction density;
		DefinteIntegral density_integral, moment_1, moment_2;

	public:
		static unsigned int MAX_SAMPLE_COUNT;

		FastEmpiricalDistribution( const module_api::pIGrid &grid,
								   module_api::uint column,
								   const DetailedApproximationMethod &method );

		double operator()( double x ) const override;
		double expectation( ) const override;
		double expectation( double a, double b ) const override;
		double deviation( ) const override;
		double deviation( double a, double b ) const override;
		double generate( ) override;

		~FastEmpiricalDistribution( ) override;
	};
}
