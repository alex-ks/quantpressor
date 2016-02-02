#include "CsvReader.h"

#include <RoughGrid.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using std::nan;

namespace io
{
	using namespace module_api;
	using igor::RoughGrid;
	using std::wifstream;
	using std::getline;
	using std::wstring;
	using std::vector;
	using std::stod;
	using std::move;

	CsvReader::CsvReader( unsigned int buffer_size, wchar_t column_delimiter ) : delimiter( column_delimiter ), buff_size( buffer_size )
	{
		
	}

	void CsvReader::parse_and_clear_string( std::wstring &str, std::vector<double> &current_row )
	{
		double value;

		if ( str.length( ) != 0 )
			value = stod( str );
		else
			value = nan( "" );

		current_row.push_back( value );
		str.clear( );
	}

	pIGrid CsvReader::read( const wstring &path, bool has_column_names, bool has_row_names )
	{
		wifstream in( path );
		wstring str;

		auto buffer = new wchar_t[buff_size];

		double value;

		vector<vector<double>> rows;
		vector<double> current_row;

		bool was_endl = false;

		while ( !in.eof( ) )
		{
			in.read( buffer, buff_size );
			auto read = in.gcount( );

			bool was_splitted = false;

			for ( int i = 0; i < read; ++i )
			{
				if ( buffer[i] == delimiter )
				{
					parse_and_clear_string( str, current_row );
				}
				else if ( buffer[i] == L'\r' )
				{
					if ( was_endl )
					{
						goto grid_pack;
					}

					parse_and_clear_string( str, current_row );

					rows.push_back( move( current_row ) );
					current_row.clear( );
					was_endl = true;

					if ( i + 1 != buff_size )
					{
						if ( buffer[i + 1] == L'\n' )
							++i;
					}
					else
					{
						was_splitted = true;
					}
				}
				else if ( buffer[i] == L'\n' )
				{
					if ( was_splitted )
					{
						++i;
						was_splitted = false;
					}
					else
					{
						if ( was_endl )
						{
							goto grid_pack;
						}

						//assume there were not '/r'
						parse_and_clear_string( str, current_row );

						rows.push_back( move( current_row ) );
						current_row = vector<double>( );
						was_endl = true;
					}
				}
				else
				{
					was_endl = false;
					str.push_back( buffer[i] );
				}
			}
		}

grid_pack:

		if ( str.length( ) != 0 )
		{
			value = stod( str );
			current_row.push_back( value );
			str.clear( );
		}

		if ( current_row.size( ) != 0 )
		{
			rows.push_back( move( current_row ) );
		}

		uint row_count = rows.size( );
		uint column_count = 0;
		HeapPtr<RoughGrid> result = nullptr;
		
		if ( row_count != 0 )
		{
			column_count = rows[0].size( );

			result = make_heap_aware<RoughGrid>( row_count, column_count );

			for ( int i = 0; i < row_count; ++i )
			{
				if ( column_count != rows[i].size( ) )
				{
					throw rows[i].size( );
				}

				for ( int j = 0; j < column_count; ++j )
				{
					result->set_value( i, j, rows[i][j] );
				}
			}
		}

		delete buffer;
		return result;
	}

	CsvReader::~CsvReader( )
	{
		
	}
}
