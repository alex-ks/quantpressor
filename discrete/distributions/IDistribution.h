#pragma once

namespace distributions
{
	class IDistribution
	{
	public:
		virtual double operator()( double x ) const = 0;

		virtual double expectation( ) const = 0;
		virtual double expectation( double a, double b ) const = 0;

		virtual double deviation( ) const = 0;
		virtual double deviation( double a, double b ) const = 0;

		virtual double generate( ) = 0;

		virtual ~IDistribution( ) { }
	};
}
