#pragma once

#include "IIntegrator.h"

namespace integrators
{
	class TrapezoidalIntegrator : public IIntegrator
	{
	private:
		double step;

	public:
		explicit TrapezoidalIntegrator( double step );

		double integrate( const RealFunction &f, double left, double right ) override;
		~TrapezoidalIntegrator( ) override;
	};
}