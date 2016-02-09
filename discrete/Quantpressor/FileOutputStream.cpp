#include "FileOutputStream.h"

#include <vcclr.h>

namespace Quantpressor
{
	namespace FileIO
	{
		grid_compressor::FileOutputStream *FileOutputStream::NativePtr( )
		{
			if ( nativeStream == nullptr )
				throw gcnew System::ObjectDisposedException( this->ToString( ) );
			return nativeStream;
		}

		FileOutputStream::FileOutputStream( System::String ^ fileName )
		{
			pin_ptr<const wchar_t> wpath = PtrToStringChars( fileName );
			nativeStream = new grid_compressor::FileOutputStream( wpath );
		}

		FileOutputStream::~FileOutputStream( )
		{
			delete nativeStream;
			nativeStream = nullptr;
		}

		FileOutputStream::!FileOutputStream( )
		{
			if ( nativeStream != nullptr )
			{
				delete nativeStream;
				nativeStream = nullptr;
			}
		}

		void FileOutputStream::Write( int i )
		{
			auto &stream = *nativeStream;
			stream << i;
		}

		void FileOutputStream::Write( System::Int64 i )
		{
			auto &stream = *nativeStream;
			stream << static_cast<unsigned long long>( i );
		}

		void FileOutputStream::Write( System::Byte b )
		{
			auto &stream = *nativeStream;
			stream << b;
		}

		void FileOutputStream::Write( double d )
		{
			auto &stream = *nativeStream;
			stream << d;
		}

		void FileOutputStream::WriteBit( bool bit )
		{
			nativeStream->write_bit( bit );
		}

		void FileOutputStream::WriteBytes( array<System::Byte>^ bytes, int count )
		{
			if ( count > bytes->Length )
				throw gcnew System::IndexOutOfRangeException( );

			pin_ptr<System::Byte> ptr = &bytes[0];
			nativeStream->write_bytes( ptr, count );
		}

		void FileOutputStream::Flush( )
		{
			nativeStream->flush( );
		}
	}
}