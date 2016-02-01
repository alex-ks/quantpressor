#pragma once

#include <io.h>
#include <functional>

#include "IDistribution.h"
#include "IIntegrator.h"

namespace distributions
{
	struct ApproximationMethod
	{
		integrators::RealFunction kernel;
		double window;
	};

	class EmpiricalDistribution : public IDistribution
	{
	private:
		integrators::RealFunction density;
		double min, max, error, ex_cache, dx_cache;

		double integrate( const integrators::RealFunction &func, double left, double right ) const;

	public:
		static unsigned int MAX_SAMPLE_COUNT;

		EmpiricalDistribution( const module_api::pIGrid &grid,
							   module_api::uint column,
							   const ApproximationMethod &method,
							   double min_value,
							   double max_value,
							   double error_order );

		double operator()( double x ) const override;

		double expectation( ) const override;
		double expectation( double a, double b ) const override;

		double deviation( ) const override;
		double deviation( double a, double b ) const override;

		double generate( ) override;
	};
}