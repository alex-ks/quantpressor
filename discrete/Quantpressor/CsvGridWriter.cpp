#include "CsvGridWriter.h"

#include "NativeGrid.h"

#include <vcclr.h>
#include <exception.h>

namespace Quantpressor
{
	namespace FileIO
	{
		CsvGridWriter::CsvGridWriter( wchar_t delimeter )
		{
			writer = new quantpressor::io::CsvWriter( delimeter );
		}

		CsvGridWriter::~CsvGridWriter( )
		{
			if ( writer == nullptr )
				throw gcnew System::ObjectDisposedException( this->ToString( ) );
			delete writer;
			writer = nullptr;
		}

		CsvGridWriter::!CsvGridWriter( )
		{
			if ( writer != nullptr )
			{
				delete writer;
				writer = nullptr;
			}
		}

		void CsvGridWriter::Write( System::String ^ fileName, IGrid ^ grid )
		{
			if ( writer == nullptr )
				throw gcnew System::ObjectDisposedException( this->ToString( ) );

			pin_ptr<const wchar_t> name = PtrToStringChars( fileName );

			auto wrapper = dynamic_cast< NativeGrid ^ >( grid );
			if ( wrapper == nullptr )
				throw gcnew System::ArgumentException( "Can handle only native grids" );

			try
			{
				auto native = wrapper->NativePtr( );
				writer->write( name, native );
			}
			catch ( module_api::Exception &e )
			{
				throw gcnew System::Exception( gcnew System::String( e.get_message( ).c_str( ) ) );
			}
		}
	}
}