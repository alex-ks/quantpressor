#include "TripletHandlers.h"

namespace quantpressor
{
	namespace compressors
	{
		using std::map;
		using std::vector;
		using std::ceil;
		using std::log2;
		using huffman::DynamicHuffmanTree;

		TripletWriter::TripletWriter( IBinaryOutputStream & stream, width_t w ) : 
			out( stream ), 
			width( w ),
			letter_number( 1 )
		{
			length_tree = nullptr;
		}

		TripletWriter::~TripletWriter( )
		{
			flush( );
			delete length_tree;
		}

		inline map<width_t, ull> calc_frequences( const vector<width_t> &cache )
		{
			map<width_t, ull> result;

			for ( auto value : cache )
			{
				if ( result.find( value ) != result.end( ) )
				{
					++result[value];
				}
				else
				{
					result[value] = 1;
				}
			}

			return std::move( result );
		}

		inline void write_code( const bit_set &code, IBinaryOutputStream &stream )
		{
			for ( auto bit : code )
			{
				stream.write_bit( bit );
			}
		}

		bit_set TripletWriter::encode_dist( width_t distance )
		{
			bit_set code;
			int bit_count = ceil( log2( letter_number ) );
			for ( int i = bit_count - 1; i >= 0; --i )
			{
				code.push_back( ( 1 << i ) & distance );
			}
			return std::move( code );
		}

		void TripletWriter::write_triplet( width_t distance, width_t length, const bit_set & code )
		{
			if ( length_tree == nullptr )
			{
				dist_cache.push_back( distance );
				length_cache.push_back( length );
				symbol_cache.push_back( code );

				if ( dist_cache.size( ) >= width )
				{
					flush( );
					dist_cache.clear( );
					length_cache.clear( );
					symbol_cache.clear( );
				}
			}
			else
			{
				auto dist_code = encode_dist( distance );
				length_tree->add_new_symbol( length );

				write_code( dist_code, out );
				write_code( length_tree->get_encoding( length ), out );
				write_code( code, out );

				letter_number += length + 1;
			}
		}

		void TripletWriter::flush( )
		{
			if ( length_tree == nullptr )
			{
				auto length_freq = calc_frequences( length_cache );

				length_tree = new DynamicHuffmanTree<width_t>( length_freq, WIDTH_T_EMPTY );

				length_tree->serialize( out );

				for ( int i = 0; i < dist_cache.size( ); ++i )
				{
					auto dist_code = encode_dist( dist_cache[i] );
					write_code( dist_code, out );
					write_code( length_tree->get_encoding( length_cache[i] ), out );
					write_code( symbol_cache[i], out );
					letter_number += length_cache[i] + 1;
				}
			}
		}
		
		using huffman::HuffmanTree;

		TripletReader::TripletReader( IBinaryInputStream &stream, width_t w, vector<huffman::HuffmanTree<double>> &&trees ) :
			in( stream ),
			symbol_trees( std::move( trees ) ),
			width( w ),
			letter_number( 1 )
		{
			length_tree = DynamicHuffmanTree<width_t>::deserialize( stream, WIDTH_T_EMPTY );
		}

		template <typename T> inline T read_compressed_value( HuffmanTree<T> &tree, IBinaryInputStream &stream )
		{
			const T *symbol_ptr = nullptr;
			while ( symbol_ptr == nullptr )
			{
				symbol_ptr = tree.make_step( stream.read_bit( ) );
			}
			return *symbol_ptr;
		}

		inline void update_frequence( map<width_t, ull> freqs, width_t value )
		{
			if ( freqs.find( value ) != freqs.end( ) )
			{
				++freqs[value];
			}
			else
			{
				freqs[value] = 1;
			}
		}

		void TripletReader::read_triplet( width_t *distance, width_t *length, double *value )
		{
			int bit_count = ceil( log2( letter_number ) );
			width_t dist = 0;

			for ( int i = 0; i < bit_count; ++i )
			{
				auto bit = in.read_bit( );
				dist <<= 1;
				dist += bit ? 1 : 0;
			}

			auto len = read_compressed_value( length_tree, in );

			curr_column = ( curr_column + len ) % symbol_trees.size( );

			auto symbol = read_compressed_value( symbol_trees[curr_column], in );

			if ( curr_width < width )
			{
				update_frequence( length_freq, len );
				
				if ( ++curr_width == width )
				{
					length_tree.assume_frequences( length_freq );
				}
			}
			else
			{
				if ( len == WIDTH_T_EMPTY )
				{ in >> len; }
				
				length_tree.add_new_symbol( len );
			}

			*distance = dist;
			*length = len;
			*value = symbol;

			letter_number += len + 1;
			curr_column = ++curr_column % symbol_trees.size( );
		}
	}
}