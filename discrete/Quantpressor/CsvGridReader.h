#pragma once
#include "Quantpressor.h"

#include <CsvReader.h>

namespace Quantpressor
{
	namespace FileIO
	{
		public ref class CsvGridReader : public IGridReader
		{
		private:
			io::CsvReader *reader;

		public:
			CsvGridReader( int bufferSize, System::Char columnDelimeter );
			~CsvGridReader( );
			!CsvGridReader( );

			virtual IGrid ^Read( System::String ^fileName,
								 bool hasRowNames,
								 bool hasColumnNames );
		};
	}
}