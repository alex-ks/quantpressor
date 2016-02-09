#include "NativeGrid.h"

module_api::pIGrid Quantpressor::NativeGrid::NativePtr( )
{
	return *nativeGrid;
}

Quantpressor::NativeGrid::NativeGrid( module_api::pIGrid &&grid )
{
	nativeGrid = new module_api::pIGrid;
	*nativeGrid = std::move( grid );
}

Quantpressor::NativeGrid::!NativeGrid( )
{
	delete nativeGrid;
	nativeGrid = nullptr;
}

double Quantpressor::NativeGrid::GetValue( int row, int column )
{
	return ( *nativeGrid )->get_value( row, column );
}

int Quantpressor::NativeGrid::RowCount::get( )
{
	return ( *nativeGrid )->get_row_count( );
}

int Quantpressor::NativeGrid::ColumnCount::get( )
{
	return ( *nativeGrid )->get_column_count( );
}