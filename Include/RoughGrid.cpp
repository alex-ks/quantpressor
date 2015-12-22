#include "RoughGrid.h"
#include "RoughMetadata.h"
#include "memory.h"
#include <wchar.h>

using namespace igor;

using module_api::DoubleArray;
using module_api::HeapPtr;
using module_api::make_heap_aware;

RoughGrid::RoughGrid( int row_count, int column_count )
{
	grid = new double*[row_count];

	for ( int i = 0; i < row_count; ++i )
		grid[i] = new double[column_count];

	this->row_count = row_count;
	this->column_count = column_count;

	metadata = make_heap_aware<RoughMetadata>( row_count, column_count );
}

RoughGrid::RoughGrid( const IGrid& origin )
{
	row_count = origin.get_row_count( );
	column_count = origin.get_column_count( );

	grid = new double*[row_count];

	for ( uint i = 0; i < row_count; ++i )
	{
		grid[i] = new double[column_count];
		for ( uint j = 0; j < column_count; ++j )
			grid[i][j] = origin.get_value( i, j );
	}

	if ( origin.get_metadata( ) != nullptr )
		metadata = make_heap_aware<RoughMetadata>( *origin.get_metadata( ) );
	else
		metadata = nullptr;
}

RoughGrid::~RoughGrid( )
{
	for ( uint i = 0; i < row_count; ++i )
		delete[] grid[i];

	delete[] grid;
}

uint RoughGrid::get_column_count( ) const
{
	return column_count;
}

uint RoughGrid::get_row_count( ) const
{
	return row_count;
}

double RoughGrid::get_value( uint i, uint j ) const
{
	if ( i >= row_count || j >= column_count )
		throw module_api::InputException::INDEX_OUT_OF_RANGE;

	return grid[i][j];
}

DoubleArray RoughGrid::get_rect( uint start_i, uint start_j, uint row_count, uint column_count ) const
{
	if ( start_i + row_count > this->row_count || start_j + column_count > this->column_count )
		throw module_api::InputException::INDEX_OUT_OF_RANGE;

	DoubleArray result( row_count * column_count );

	for ( uint i = 0; i < row_count; ++i )
		for ( uint j = 0; j < column_count; ++j )
			result[i * column_count + j] = grid[i + start_i][j + start_j];

	return result;
}

module_api::pIMetadata RoughGrid::get_metadata( ) const
{
	return metadata;
}

void RoughGrid::set_value( uint i, uint j, double value )
{
	if ( i >= row_count || j >= column_count )
		throw module_api::InputException::INDEX_OUT_OF_RANGE;

	grid[i][j] = value;
}

void igor::RoughGrid::set_metadata( const module_api::pIMetadata &new_metadata )
{
	metadata = new_metadata;
}
