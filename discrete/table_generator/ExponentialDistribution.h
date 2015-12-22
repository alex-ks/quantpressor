#pragma once

#include "lib.h"

#include <random>

namespace grid_generator
{
	class ExponentialDistribution : public IDistribution
	{
	private:
		double lambda;
		std::random_device random_source;
		mutable std::exponential_distribution<double> distr;

	public:
		ExponentialDistribution( double lambda );

		virtual double generate( ) override;

		virtual double operator()( double x ) const override;

		virtual double expectation( ) const override;
		virtual double expectation( double a, double b ) const override;
	};
}