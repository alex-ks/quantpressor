#include "CsvGridReader.h"
#include "NativeGrid.h"

#include <vcclr.h>
#include <exception.h>

namespace Quantpressor
{
	namespace FileIO
	{
		CsvGridReader::CsvGridReader( int bufferSize, System::Char columnDelimeter )
		{
			reader = new quantpressor::io::CsvReader( bufferSize, columnDelimeter );
		}

		CsvGridReader::~CsvGridReader( )
		{
			if ( reader == nullptr )
				throw gcnew System::ObjectDisposedException( this->ToString( ) );
			delete reader;
			reader = nullptr;
		}

		CsvGridReader::!CsvGridReader( )
		{
			if ( reader != nullptr )
			{
				delete reader;
				reader = nullptr;
			}
		}

		IGrid ^CsvGridReader::Read( System::String ^fileName,
									bool hasRowNames,
									bool hasColumnNames )
		{
			if ( reader == nullptr )
				throw gcnew System::ObjectDisposedException( this->ToString( ) );

			pin_ptr<const wchar_t> name = PtrToStringChars( fileName );

			try
			{
				auto result = reader->read( name, hasColumnNames, hasRowNames );
				return gcnew NativeGrid( std::move( result ) );
			}
			catch ( module_api::Exception &e )
			{
				throw gcnew System::Exception( gcnew System::String( e.get_message( ).c_str( ) ) );
			}
		}
	}
}