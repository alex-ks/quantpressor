#include "CsvWriter.h"

#include <fstream>

namespace quantpressor
{
	namespace io
	{
		using std::wofstream;
		using std::endl;

		CsvWriter::CsvWriter( wchar_t delimeter )
		{ 
			d = delimeter;
		}

		void CsvWriter::write( std::wstring path, const module_api::pIGrid & grid )
		{
			wofstream out = wofstream( path.c_str( ) );

			if ( grid->get_row_count( ) == 0 ||
				 grid->get_column_count( ) == 0 )
				return;

			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				out << grid->get_value( i, 0 );
				for ( int j = 1; j < grid->get_column_count( ); ++j )
					out << d << grid->get_value( i, j );
				out << endl;
			}
		}
	}
}
