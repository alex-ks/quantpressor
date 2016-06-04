#pragma once

#include "Quantpressor.h"

#include <CsvWriter.h>

namespace Quantpressor
{
	namespace FileIO
	{
		public ref class CsvGridWriter : public IGridWriter
		{
		private:
			quantpressor::io::CsvWriter *writer;

		public:
			CsvGridWriter( wchar_t delimeter );
			~CsvGridWriter( );
			!CsvGridWriter( );
			
			virtual void Write( System::String ^ fileName, IGrid ^ grid );
		};
	}
}
