#pragma once

#include "lib.h"

#include <random>

namespace grid_generator
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

		virtual double operator()( double x ) const override;

		virtual double expectation( ) const override;
		virtual double expectation( double a, double b ) const override;

		virtual double deviation( ) const override;
		virtual double deviation( double a, double b ) const override;

		virtual double generate( ) override;
	};
}