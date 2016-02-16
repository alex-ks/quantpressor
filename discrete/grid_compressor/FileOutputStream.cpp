#define _CRT_SECURE_NO_WARNINGS

#include "FileOutputStream.h"
#include <wchar.h>

#include <exception.h>

namespace quantpressor
{
	namespace io
	{
		FileOutputStream::FileOutputStream( std::wstring file_name ) : buffered_bits( 0 ), buffered_bits_count( 0 )
		{
			file = _wfopen( file_name.c_str( ), L"wb" );
			if ( file == nullptr )
			{
				throw module_api::Exception( L"Cannot open the file" );
			}
		}

		FileOutputStream::FileOutputStream( FileOutputStream && stream )
		{
			throw module_api::NotImplementedException( );
			file = stream.file;
			stream.file = nullptr;
		}

		FileOutputStream & FileOutputStream::operator=( FileOutputStream && stream )
		{
			throw module_api::NotImplementedException( );
			if ( this != &stream )
			{
				file = stream.file;
				stream.file = nullptr;
			}

			return *this;
		}

		FileOutputStream::~FileOutputStream( )
		{
			flush( );
			fclose( file );
		}

		void FileOutputStream::write_bytes( byte *bytes, unsigned int count )
		{
			for ( auto i = 0; i < count; ++i )
			{
				byte current_byte = bytes[i];
				for ( int j = 0; j < BITS_COUNT; ++j )
				{
					write_bit( current_byte >> 7 );
					current_byte <<= 1;
				}
			}
		}

		void FileOutputStream::flush( )
		{
			if ( buffered_bits_count != 0 )
			{
				buffered_bits <<= BITS_COUNT - buffered_bits_count;
				buffered_bytes.push_back( buffered_bits );
			}
			std::fwrite( buffered_bytes.c_str( ), sizeof( char ), buffered_bytes.size( ), file );
			buffered_bytes.clear( );
			buffered_bits = buffered_bits_count = 0;
		}

		void FileOutputStream::write_bit( bool bit )
		{
			++buffered_bits_count;
			buffered_bits <<= 1;
			buffered_bits += bit;

			if ( buffered_bits_count == BITS_COUNT )
			{
				buffered_bytes.push_back( buffered_bits );
				buffered_bits = buffered_bits_count = 0;
			}

			if ( buffered_bytes.size( ) == BUFFER_SIZE )
			{
				std::fwrite( buffered_bytes.c_str( ), sizeof( char ), buffered_bytes.size( ), file );
				buffered_bytes.clear( );
			}
		}

		IBinaryOutputStream &FileOutputStream::operator<<( int num )
		{
			return write_smth( num );
		}

		IBinaryOutputStream &FileOutputStream::operator<<( unsigned int num )
		{
			return write_smth( num );
		}

		IBinaryOutputStream &FileOutputStream::operator<<( unsigned long long num )
		{
			return write_smth( num );
		}

		IBinaryOutputStream & FileOutputStream::operator<<( char c )
		{
			return write_smth( c );
		}

		IBinaryOutputStream &FileOutputStream::operator<<( byte b )
		{
			return write_smth( b );
		}

		IBinaryOutputStream &FileOutputStream::operator<<( double num )
		{
			return write_smth( num );
		}
	}
}