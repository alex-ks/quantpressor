#pragma once

#include "IBinaryStream.h"

namespace quantpressor
{
	class IBinaryOutputStream
	{
	public:
		virtual IBinaryOutputStream &operator<<( int ) = 0;
		virtual IBinaryOutputStream &operator<<( unsigned int ) = 0;
		virtual IBinaryOutputStream &operator<<( unsigned long long ) = 0;
		virtual IBinaryOutputStream &operator<<( byte ) = 0;
		virtual IBinaryOutputStream &operator<<( double ) = 0;

		virtual void write_bit( bool bit ) = 0;
		virtual void write_bytes( byte *bytes, unsigned int count ) = 0;

		virtual void flush( ) = 0;

		virtual ~IBinaryOutputStream( ) { }
	};
}
