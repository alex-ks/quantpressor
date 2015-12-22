#include "RoughMetadata.h"
#include <cwchar>

using namespace igor;
using std::wcscpy;
using std::wcslen;
using std::wstring;
using std::vector;

using module_api::uint;
using module_api::ReplacementState;
using module_api::InputException;

RoughMetadata::RoughMetadata( uint row_count, uint column_count ) : replacements( 0 ),
																	column_names( 0 ),
																	row_names( 0 )
{
	this->column_count = column_count;
	this->row_count = row_count;

	replacements_state = ReplacementState::ABSENT;
}

RoughMetadata::RoughMetadata( const IMetadata &metadata ) : replacements( metadata.get_column_count( ) )
{
	replacements_state = metadata.get_replacements_state( );

	column_count = metadata.get_column_count( );
	row_count = metadata.get_row_count( );

	if ( replacements_state != ReplacementState::ABSENT )
	{
		int count = ReplacementState::REPLACES_COLUMNS == replacements_state ?
			metadata.get_column_count( ) :
			metadata.get_row_count( );

		replacements.resize( count );

		for ( int i = 0; i < count; ++i )
			replacements[i] = metadata.get_replacement( i );
	}

	column_names = metadata.get_column_names( );
	row_names = metadata.get_row_names( );
}

RoughMetadata::~RoughMetadata( )
{

}

ReplacementState igor::RoughMetadata::get_replacements_state( ) const
{
	return replacements_state;
}

wstring igor::RoughMetadata::get_column_name( uint column ) const
{
	if ( column_names.size( ) == 0 )
		return L"";

	if ( column < column_count )
		return column_names[column];
	else
		throw InputException::INDEX_OUT_OF_RANGE;
}

const std::vector<std::wstring> & igor::RoughMetadata::get_column_names( ) const
{
	return column_names;
}

wstring igor::RoughMetadata::get_row_name( uint row ) const
{
	if ( row_names.size( ) == 0 )
		return L"";

	if ( row < row_count )
		return row_names[row];
	else
		throw InputException::INDEX_OUT_OF_RANGE;
}

const std::vector<wstring> &igor::RoughMetadata::get_row_names( ) const
{
	return row_names;
}

uint igor::RoughMetadata::get_row_count( ) const
{
	return row_count;
}

const std::vector<wstring> &RoughMetadata::get_replacement( uint index ) const
{
	if ( replacements_state != ReplacementState::ABSENT )
		return replacements[index];
	throw InputException::UNKNOWN;
}

void igor::RoughMetadata::set_row_name( uint row_num, const wstring &new_name )
{
	if (row_names.size( ) == 0)
	{
		row_names.resize(row_count);

		for ( uint i = 0; i < row_count; ++i )
			row_names[i] = L"";
	}

	if ( row_num < row_count )
		row_names[row_num] = new_name;
	else
		throw InputException::INDEX_OUT_OF_RANGE;
}

void igor::RoughMetadata::set_row_names( const std::vector<std::wstring>& names )
{
	row_names = names;
}

void igor::RoughMetadata::set_column_name( uint column_num, const wstring &new_name )
{
	if ( column_names.size( ) == 0 )
	{
		column_names.resize( column_count );

		for ( uint i = 0; i < column_count; ++i )
			column_names[i] = L"";
	}

	if ( column_num < column_count )
		column_names[column_num] = new_name;
	else
		throw InputException::INDEX_OUT_OF_RANGE;
}

void igor::RoughMetadata::set_column_names( const std::vector<std::wstring>& names )
{
	column_names = names;
}

void igor::RoughMetadata::set_replacement( uint column_num, const vector<wstring> &new_replacement )
{
	replacements[column_num] = new_replacement;
}

void igor::RoughMetadata::set_replacemnts_state( ReplacementState new_state )
{
	replacements_state = new_state;
	switch ( replacements_state )
	{
	case ReplacementState::REPLACES_ROWS:
		replacements.clear( );
		replacements.resize( row_count );
		break;

	case ReplacementState::REPLACES_COLUMNS:
		replacements.clear( );
		replacements.resize( column_count );
		break;

	case ReplacementState::ABSENT:
		replacements.clear( );
		break;
	}
}

uint RoughMetadata::get_column_count( ) const
{
	return column_count;
}