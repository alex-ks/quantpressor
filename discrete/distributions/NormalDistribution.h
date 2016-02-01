#pragma once

#include "IDistribution.h"

#include <random>

namespace distributions
{
	class NormalDistribution : public IDistribution
	{
	private:
		const double root_factor;
		double alpha, sigma;
		std::random_device random_source;
		std::normal_distribution<double> distr;

	public:
		NormalDistribution( double mean, double deviation );

		double operator()( double x ) const override;

		double expectation( ) const override;
		double expectation( double a, double b ) const override;

		double deviation( ) const override;
		double deviation( double a, double b ) const override;

		double generate( ) override;
	};
}