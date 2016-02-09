#pragma once

#include "INativeDistribution.h"

#include <FastEmpiricalDistribution.h>

namespace Quantpressor
{
	namespace Distributions
	{
		public ref class EmpiricalDistribution : public INativeDistribution
		{
		private:
			distributions::FastEmpiricalDistribution *nativeDistribution;

		public:
			EmpiricalDistribution( IGrid ^grid, int column );
			EmpiricalDistribution( IGrid ^grid, int column, int maxSampleCount );

			!EmpiricalDistribution( );

			virtual double Density( double x );
			virtual double Cumulative( double x );

			virtual property double TotalExpectation
			{
				double get( );
			}
			virtual double Expectation( double a, double b );

			virtual property double TotalVariance
			{
				double get( );
			}
			virtual double Variance( double a, double b );

			virtual distributions::IDistribution *NativePtr( );
		};
	}
}