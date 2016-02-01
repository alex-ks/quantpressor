#pragma once

#include <functional>

namespace integrators
{
	typedef std::function<double( double )> RealFunction;

	class IIntegrator
	{
	public:
		virtual double integrate( const RealFunction &f, double left, double right ) = 0;

		virtual ~IIntegrator( ) { }
	};
}
