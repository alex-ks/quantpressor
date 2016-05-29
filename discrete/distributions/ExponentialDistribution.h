#pragma once

#include "IDistribution.h"

#include <random>

namespace distributions
{
	class ExponentialDistribution : public IDistribution
	{
	private:
		double lambda;
		std::random_device random_source;
		mutable std::exponential_distribution<double> distr;

	public:
		ExponentialDistribution( double lambda );

		double generate( ) override;

		double operator()( double x ) const override;

		double expectation( ) const override;
		double expectation( double a, double b ) const override;

		double density( double x ) const override;
		double deviation( ) const override;
		double deviation( double a, double b ) const override;
	};
}