#pragma once
#include "IBinaryOutputStream.h"

#include <string>
#include <cstdio>

namespace quantpressor
{
	namespace io
	{
		class FileOutputStream : public IBinaryOutputStream
		{
		private:
			static const unsigned int BUFFER_SIZE = 1024;
			static const unsigned int BITS_COUNT = 8;

			std::FILE *file;
			byte buffered_bits;
			int buffered_bits_count;
			std::string buffered_bytes;

			template<typename T> inline IBinaryOutputStream &write_smth( T smth )
			{
				auto bytes = reinterpret_cast<byte*>( &smth );
				write_bytes( bytes, sizeof( T ) );
				return *this;
			}

			FileOutputStream( const FileOutputStream & ) = delete;

		public:
			FileOutputStream( std::wstring file_name );

			FileOutputStream( FileOutputStream &&stream );
			FileOutputStream &operator=( FileOutputStream &&stream );

			~FileOutputStream( );

			virtual IBinaryOutputStream & operator<<( int ) override;
			virtual IBinaryOutputStream & operator<<( unsigned int ) override;
			virtual IBinaryOutputStream & operator<<( unsigned long long ) override;
			virtual IBinaryOutputStream & operator<<( char ) override;
			virtual IBinaryOutputStream & operator<<( byte ) override;
			virtual IBinaryOutputStream & operator<<( double ) override;
			virtual void write_bit( bool bit ) override;
			virtual void write_bytes( byte *bytes, unsigned int count ) override;

			virtual void flush( ) override;
		};
	}
}
