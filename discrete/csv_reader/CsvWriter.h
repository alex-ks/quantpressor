#pragma once

#include "IGridWriter.h"

namespace quantpressor
{
	namespace io
	{
		class CsvWriter : public IGridWriter
		{
		private:
			wchar_t d;

		public:
			CsvWriter( wchar_t delimeter );

			void write( std::wstring path, const module_api::pIGrid & grid ) override;
		};
	}
}
