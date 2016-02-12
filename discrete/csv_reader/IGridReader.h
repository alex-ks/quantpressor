#pragma once

#include <io.h>
#include <string>

namespace quantpressor
{
	class IGridReader
	{
	public:
		virtual module_api::pIGrid read( const std::wstring &path, bool has_column_names, bool has_row_names ) = 0;

		virtual ~IGridReader( ) { }
	};
}