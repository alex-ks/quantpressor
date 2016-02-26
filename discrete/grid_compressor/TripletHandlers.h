#pragma once
#include "huffman.h"
#include "IBinaryInputStream.h"
#include "IBinaryOutputStream.h"

#include <exception.h>

namespace quantpressor
{
	namespace compressors
	{
		typedef long long index_t;
		typedef int width_t;

		class TripletWriter
		{
		private:
			IBinaryOutputStream &out;

		public:
			TripletWriter( IBinaryOutputStream &stream );

			void write_triplet( width_t distance, width_t length, const bit_set &code );
		};

		class TripletReader
		{
		private:

		public:
		};
	}
}