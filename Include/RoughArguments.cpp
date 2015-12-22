#include "RoughArguments.h"
#include "RoughGrid.h"

#include "memory.h"

using namespace igor;

using module_api::make_heap_aware;
using module_api::pIGrid;
using module_api::uint;

igor::RoughArguments::RoughArguments( )
{
	
}

igor::RoughArguments::~RoughArguments( )
{

}

igor::RoughArguments::RoughArguments( const IArguments & origin )
{
	for ( uint i = 0; i < origin.get_grid_count( ); ++i )
		grids.push_back( make_heap_aware<RoughGrid>( *origin.get_grid( i ) ) );
	analytics_flag = origin.make_analytics( );
}

pIGrid igor::RoughArguments::get_grid( uint number ) const
{
	if ( number < 0 || number > grids.size( ) )
		throw module_api::InputException::INDEX_OUT_OF_RANGE;
	return grids[number];
}

uint igor::RoughArguments::get_grid_count( ) const
{
	return grids.size( );
}

void igor::RoughArguments::add_grid( const pIGrid &grid )
{ 
	grids.push_back( grid );
}

bool igor::RoughArguments::make_analytics( ) const
{
	return analytics_flag;
}
