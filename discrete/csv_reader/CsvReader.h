#pragma once

#include "IGridReader.h"

namespace quantpressor
{
	namespace io
	{
		class CsvReader : public IGridReader
		{
			wchar_t delimiter;
			unsigned int buff_size;

			void parse_and_clear_string( std::wstring &str, std::vector<double> &current_row );

		public:
			CsvReader( unsigned int buffer_size, wchar_t column_delimiter );

			module_api::pIGrid read( const std::wstring &path, bool has_column_names, bool has_row_names ) override;

			~CsvReader( ) override;
		};
	}
}
