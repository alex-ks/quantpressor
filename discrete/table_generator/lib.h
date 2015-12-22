#pragma once

#include <io.h>
#include <memory.h>

#include <vector>

namespace grid_generator
{
	using module_api::HeapPtr;
	using module_api::make_heap_aware;

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

	typedef HeapPtr<IDistribution> pIDistribution;

	module_api::pIGrid generate_grid( module_api::uint row_count, std::vector<pIDistribution> column_distributions );
}