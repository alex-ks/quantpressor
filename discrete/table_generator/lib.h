#pragma once

#include <io.h>
#include <memory.h>

#include <vector>

#include <IDistribution.h>

namespace grid_generator
{
	using module_api::HeapPtr;
	using module_api::make_heap_aware;			

	typedef HeapPtr<distributions::IDistribution> pIDistribution;

	module_api::pIGrid generate_grid( module_api::uint row_count, std::vector<pIDistribution> column_distributions );
}