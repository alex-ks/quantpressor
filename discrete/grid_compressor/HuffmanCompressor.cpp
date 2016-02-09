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

	HuffmanCompressor::ColumnInfo HuffmanCompressor::compress_column( uint column,
																	  const pIGrid &grid,
																	  const Quantization &quantization,
																	  const IBinaryOutputStream &s )
	{
		ColumnInfo result = ColumnInfo( );
		result.min = std::numeric_limits<double>::max( );
		result.max = std::numeric_limits<double>::min( );
		result.avg = 0;
		result.dx = 0;
		result.bps = 0;

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
			double value = grid->get_value( i, column );
			double code = find_code( value, quantization );
			bit_set coding = encoding[code];

			for ( int j = 0; j < coding.size( ); ++j )
			{
				stream.write_bit( coding[j] );
			}

			auto error = fabs( value - code );
			auto square_error = pow( value - code, 2 );

			result.max = result.max > error ? result.max : error;
			result.min = result.min < error ? result.min : error;
			result.avg = i > 0 ? ( result.avg * i + error ) / ( i + 1 ) : error;

			result.dx = i > 0 ? ( result.dx * i + square_error ) / ( i + 1 ) : square_error;
		}

		result.bps = static_cast<double>( bit_count ) / grid->get_row_count( );

		return std::move( result );
	}

	CompressionResult HuffmanCompressor::compress( const pIGrid &grid,
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

		CompressionResult result = CompressionResult( );

		for ( uint i = 0; i < grid->get_column_count( ); ++i )
		{
			auto info = compress_column( i, grid, quantizations[i], s );
			result.columns_bps.push_back( info.bps );
			result.real_variances.push_back( info.dx );
			result.min_errors.push_back( info.min );
			result.avg_errors.push_back( info.avg );
			result.max_errors.push_back( info.max );
		}

		return std::move( result );
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
