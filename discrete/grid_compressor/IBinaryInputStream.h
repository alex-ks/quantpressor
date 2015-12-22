#pragma once

#include "IBinaryStream.h"

namespace grid_compressor
{
	class IBinaryInputStream
	{
	public:
		virtual IBinaryInputStream &operator>>( int & ) = 0;
		virtual IBinaryInputStream &operator>>( unsigned int & ) = 0;
		virtual IBinaryInputStream &operator>>( unsigned long long & ) = 0;
		virtual IBinaryInputStream &operator>>( byte & ) = 0;
		virtual IBinaryInputStream &operator>>( double & ) = 0;

		virtual bool read_bit( ) = 0;
		virtual unsigned int read_bytes( byte *bytes, unsigned int count ) = 0;

		virtual bool end_of_stream( ) = 0;

		virtual ~IBinaryInputStream( ) { }
	};
}