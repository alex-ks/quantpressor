#pragma once

namespace grid_compressor
{
	typedef unsigned char byte;
	typedef unsigned long long ull;

	//todo: inherit i/o streams from this
	class IBinaryStream
	{
	public:
		virtual ull get_current_position( ) const = 0;

		virtual ~IBinaryStream( ) { }
	};
}