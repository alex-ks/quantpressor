#define _CRT_SECURE_NO_WARNINGS

#include "FileInputStream.h"

#include <exception.h>

namespace grid_compressor
{
	FileInputStream::FileInputStream( std::wstring file_name )
	{
		file = _wfopen( file_name.c_str( ), L"rb" );
		
		buffered_bytes = new byte[BUFFER_SIZE];

		if ( file == nullptr )
		{
			throw module_api::Exception( L"Cannot open the file" );
		}
	}

	FileInputStream::FileInputStream( FileInputStream && stream )
	{
		throw module_api::NotImplementedException( );
		file = stream.file;
		stream.file = nullptr;
	}

	FileInputStream & FileInputStream::operator=( FileInputStream && stream )
	{
		throw module_api::NotImplementedException( );
		if ( this != &stream )
		{
			file = stream.file;
			stream.file = nullptr;
		}

		return *this;
	}

	FileInputStream::~FileInputStream( )
	{
		fclose( file );
		delete buffered_bytes;
	}

	void FileInputStream::load_bytes( )
	{
		auto read = fread( buffered_bytes, sizeof( byte ), BUFFER_SIZE, file );
		buffered_count = read;
		next_unread = 0;
		buffered_bits_count = 0;

		if ( read < BUFFER_SIZE )
		{
			if ( feof( file ) != 0 )
			{
				eof = true;
			}
			else
			{
				throw module_api::Exception( L"Error reading file" );
			}
		}
	}

	inline byte FileInputStream::read_bit_as_byte( )
	{
		if ( !eof && buffered_count == next_unread && buffered_bits_count == 0 )
		{
			load_bytes( );
		}
		if ( end_of_stream( ) )
		{
			return false;
		}

		if ( buffered_bits_count == 0 )
		{
			buffered_bits = buffered_bytes[next_unread++];
			buffered_bits_count = 8;
		}

		byte result = buffered_bits >> 7;
		buffered_bits <<= 1;
		--buffered_bits_count;

		return result;
	}

	bool FileInputStream::read_bit( )
	{
		return read_bit_as_byte( );
	}

	unsigned int FileInputStream::read_bytes( byte * bytes, unsigned int count )
	{
		unsigned int read = 0;

		while ( !end_of_stream( ) && read < count )
		{
			byte curr = 0;
			for ( int i = 0; i < BITS_COUNT; ++i )
			{
				curr <<= 1;
				byte bit = read_bit_as_byte( );
				curr = ( curr + bit );
			}
			if ( !end_of_stream( ) )
			{
				bytes[read++] = curr;
			}
		}

		return read;
	}

	bool FileInputStream::end_of_stream( )
	{
		return eof && ( buffered_count == next_unread ) && ( buffered_bits_count == 0 );
	}

	IBinaryInputStream & FileInputStream::operator>>( int &number )
	{
		return read_smth( number );
	}

	IBinaryInputStream & FileInputStream::operator>>( unsigned int &number )
	{
		return read_smth( number );
	}

	IBinaryInputStream & FileInputStream::operator>>( unsigned long long &number )
	{
		return read_smth( number );
	}

	IBinaryInputStream & FileInputStream::operator>>( byte &b )
	{
		return read_smth( b );
	}

	IBinaryInputStream & FileInputStream::operator>>( double &number )
	{
		return read_smth( number );
	}
}
