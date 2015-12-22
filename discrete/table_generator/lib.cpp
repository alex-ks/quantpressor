#include "lib.h"

#include <RoughGrid.h>

using module_api::pIGrid;
using module_api::uint;
using igor::RoughGrid;

module_api::pIGrid grid_generator::generate_grid( module_api::uint row_count, std::vector<pIDistribution> column_distributions )
{
	auto result = make_heap_aware<RoughGrid>( row_count, column_distributions.size( ) );

	for ( uint i = 0; i < row_count; ++i )
		for ( uint j = 0; j < column_distributions.size( ); ++j )
			result->set_value( i, j, column_distributions[j]->generate( ) );

	return result;
}
