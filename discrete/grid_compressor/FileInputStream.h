#pragma once

#include <string>
#include <cstdio>

#include "IBinaryInputStream.h"

namespace quantpressor
{
	namespace io
	{
		class FileInputStream : public IBinaryInputStream
		{
			static const unsigned int BUFFER_SIZE = 1024;
			static const unsigned int BITS_COUNT = 8;

			std::FILE *file;
			byte buffered_bits;
			int buffered_bits_count;
			byte *buffered_bytes;
			unsigned int buffered_count, next_unread;
			bool eof = false;

			template<typename T> inline IBinaryInputStream &read_smth( T &smth )
			{
				auto bytes = reinterpret_cast<byte*>( &smth );
				read_bytes( bytes, sizeof( T ) );
				return *this;
			}

			FileInputStream( const FileInputStream & ) = delete;

			void load_bytes( );
			inline byte read_bit_as_byte( );

		public:
			FileInputStream( std::wstring file_name );

			FileInputStream( FileInputStream &&stream );
			FileInputStream &operator=( FileInputStream &&stream );

			~FileInputStream( );

			virtual IBinaryInputStream & operator>>( int & ) override;
			virtual IBinaryInputStream & operator>>( unsigned int & ) override;
			virtual IBinaryInputStream & operator>>( unsigned long long & ) override;
			virtual IBinaryInputStream & operator>>( byte & ) override;
			virtual IBinaryInputStream & operator>>( double & ) override;

			virtual bool read_bit( ) override;
			virtual unsigned int read_bytes( byte * bytes, unsigned int count ) override;

			virtual bool end_of_stream( ) override;
		};
	}
}
