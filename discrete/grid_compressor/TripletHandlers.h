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

		const width_t WIDTH_T_EMPTY = -1;

		class TripletWriter
		{
		private:
			IBinaryOutputStream &out;
			std::vector<width_t> dist_cache, length_cache;
			std::vector<bit_set> symbol_cache;
			width_t width;
			huffman::DynamicHuffmanTree<width_t> *dist_tree, *length_tree;

		public:
			TripletWriter( IBinaryOutputStream &stream, width_t width );
			~TripletWriter( );

			void write_triplet( width_t distance, width_t length, const bit_set &code );
			void flush( );
		};

		class TripletReader
		{
		private:
			IBinaryInputStream &in;
			std::vector<huffman::HuffmanTree<double>> symbol_trees;
			std::map<width_t, ull> dist_freq, length_freq;
			unsigned int curr_column = 0;
			width_t width, curr_width = 0;
			huffman::DynamicHuffmanTree<width_t> dist_tree, length_tree;

		public:
			TripletReader( IBinaryInputStream &stream, width_t width, std::vector<huffman::HuffmanTree<double>> &&trees );

			void read_triplet( width_t *distance, width_t *length, double *value );
		};
	}
}