#pragma once

#include "IInputReader.h"

namespace io
{
	class CsvReader : public IInputReader
	{
		wchar_t delimiter;
		unsigned int buff_size;

	public:
		CsvReader( unsigned int buffer_size, wchar_t column_delimiter );

		module_api::pIGrid read( const std::wstring &path, bool has_column_names, bool has_row_names ) override;

		~CsvReader( ) override;
	};
}

