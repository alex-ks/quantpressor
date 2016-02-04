#include "TrapezoidalIntegrator.h"

namespace integrators
{
	TrapezoidalIntegrator::TrapezoidalIntegrator( double step )
	{
		this->step = step;
	}

	double TrapezoidalIntegrator::integrate( const RealFunction &f, double left, double right )
	{
		double result = f( left ) * 0.5;

		for ( double x = left + step; x < right; x += step )
		{
			result += f( x );
		}

		step = step < right - left ? step : right - left;

		result += f( right ) * 0.5;
		result *= step;

		return result;
	}

	TrapezoidalIntegrator::~TrapezoidalIntegrator( )
	{
		
	}
}
