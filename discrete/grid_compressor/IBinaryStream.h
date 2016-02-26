#pragma once

namespace quantpressor
{
	typedef unsigned char byte;
	typedef unsigned long long ull;

	class IBinaryStream
	{
	public:
		virtual ull get_current_position( ) const = 0;

		virtual ~IBinaryStream( ) { }
	};
}