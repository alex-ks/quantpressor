#include "HuffmanCompressor.h"
#include "TreeNode.h"

#include <RoughGrid.h>

#include <exception.h>
#include <utility>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <iostream>

#include "utilities.h"

namespace huffman
{
	using namespace grid_compressor;

	using module_api::uint;
	using module_api::pIGrid;
	using igor::RoughGrid;

	using std::map;
	using std::vector;
	using std::string;
	using std::priority_queue;

	typedef TreeNode<double> Node;
	typedef vector<bool> bit_set;

	static void create_via_dfs( map<double, bit_set> &encoding, bit_set code, Node *node )
	{
		if ( node->stores_value( ) )
		{
			encoding[node->get_value( )] = code;
			return;
		}

		code.push_back( 1 );

		if ( node->get_one( ) != nullptr )
		{
			create_via_dfs( encoding, code, node->get_one( ) );
		}
		if ( node->get_zero( ) != nullptr )
		{
			code[code.size( ) - 1] = 0;
			create_via_dfs( encoding, code, node->get_zero( ) );
		}
	}

	static void pack_tree( Node *root, IBinaryOutputStream &stream )
	{
		if ( root == nullptr )
			return;

		if ( root->stores_value( ) )
		{
			stream.write_bit( 0 );
			stream << root->get_value( );
			return;
		}

		stream.write_bit( 1 );
		pack_tree( root->get_zero( ), stream );
		pack_tree( root->get_one( ), stream );
	}

	static map<double, bit_set> create_encoding( Node *root )
	{
		map<double, bit_set> result;
		if ( root == nullptr )
			return result;
		bit_set empty_code;
		create_via_dfs( result, empty_code, root );
		return std::move( result );
	}

	double HuffmanCompressor::compress_column( uint column,
											   const pIGrid &grid,
											   const Quantization &quantization,
											   const IBinaryOutputStream &s )
	{
		map<double, ull> counts;
		auto &stream = const_cast< IBinaryOutputStream & >( s );

		for ( double code : quantization.codes )
		{
			counts[code] = 0UL;
		}

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{
			++counts[find_code( grid->get_value( i, column ), quantization )];
		}

		priority_queue<Node *, vector<Node *>, less<double>> queue;

		for ( auto &pair : counts )
		{
			if ( pair.second != 0UL )
				queue.push( new Node( pair.first, pair.second ) );
		}

		Node *tree_root;

		if ( queue.size( ) == 1 )
		{
			auto stub = new Node( std::numeric_limits<double>::infinity( ), 0ULL );
			tree_root = new Node( stub, queue.top( ) );
		}
		else
		{
			while ( queue.size( ) > 1 )
			{
				Node *one, *zero;
				one = queue.top( );
				queue.pop( );
				zero = queue.top( );
				queue.pop( );
				queue.push( new Node( one, zero ) );
			}
			tree_root = queue.top( );
		}

		auto encoding = create_encoding( tree_root );

		ull bit_count = 0UL;

		for ( auto &pair : counts )
		{
			if ( pair.second != 0UL )
				bit_count += pair.second * encoding[pair.first].size( );
		}

		pack_tree( tree_root, stream );

		stream << bit_count;

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{
			bit_set coding = encoding[find_code( grid->get_value( i, column ), quantization )];

			for ( int j = 0; j < coding.size( ); ++j )
			{
				stream.write_bit( coding[j] );
			}
		}

		return static_cast< double >( bit_count ) / grid->get_row_count( );
	}

	vector<double> HuffmanCompressor::compress( const pIGrid &grid,
												const Quantizations &quantizations,
												const IBinaryOutputStream &s )
	{
		auto &stream = const_cast< IBinaryOutputStream & >( s );
		stream << grid->get_row_count( );
		stream << grid->get_column_count( );

		if ( grid->get_column_count( ) != quantizations.size( ) )
		{
			throw module_api::BadInputException( L"Quantizations count must be equal to column count" );
		}

		vector<double> bps_counts;

		for ( uint i = 0; i < grid->get_column_count( ); ++i )
		{
			auto bps = compress_column( i, grid, quantizations[i], s );
			bps_counts.push_back( bps );
		}

		return std::move( bps_counts );
	}

	static Node *unpack_tree( IBinaryInputStream &stream )
	{
		bool bit = stream.read_bit( );

		if ( bit )// == 1
		{
			auto zero = unpack_tree( stream );
			auto one = unpack_tree( stream );
			return new Node( one, zero );
		}
		else // == 0
		{
			double value;
			stream >> value;
			return new Node( value, 0ULL );
		}
	}

	module_api::pIGrid HuffmanCompressor::decompress( const grid_compressor::IBinaryInputStream &s )
	{
		auto &stream = const_cast< IBinaryInputStream & > ( s );
		unsigned int row_count, column_count;
		stream >> row_count;
		stream >> column_count;

		auto grid = module_api::make_heap_aware<RoughGrid>( row_count, column_count );

		for ( int column = 0; column < column_count; ++column )
		{
			auto root = unpack_tree( stream );

			auto enc = create_encoding( root );

			ull bit_count;
			stream >> bit_count;

			Node *curr = root;
			uint row = 0;

			for ( ull i = 0; i < bit_count; ++i )
			{
				auto bit = stream.read_bit( );

				if ( bit )
					curr = curr->get_one( );
				else
					curr = curr->get_zero( );

				if ( curr->stores_value( ) )
				{
					grid->set_value( row++, column, curr->get_value( ) );
					curr = root;
				}
			}
		}

		return grid;
	}
}
