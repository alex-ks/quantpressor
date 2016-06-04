#pragma once

#include <io.h>
#include <string>

namespace quantpressor
{
	class IGridWriter
	{
	public:
		virtual void write( std::wstring path, const module_api::pIGrid &grid ) = 0;

		virtual ~IGridWriter( ) { }
	};
}
