#pragma once
#include <map>
#include <queue>
#include <vector>

#include "IBinaryInputStream.h"
#include "IBinaryOutputStream.h"

namespace quantpressor
{
	namespace compressors
	{
		typedef unsigned long long ull;
		typedef std::vector<bool> bit_set;

		template <typename T> class HuffmanTree;

		template <typename T> class TreeNode
		{
		private:
			T value;
			ull priority;
			bool is_leaf;
			TreeNode *one, *zero;

			friend HuffmanTree<T>;

		public:
			TreeNode( TreeNode *one, TreeNode *zero ) :
				one( one ),
				zero( zero ),
				value( )
			{
				is_leaf = false;
				priority = 0UL;

				if ( one != nullptr )
					priority += one->priority;
				if ( zero != nullptr )
					priority += zero->priority;
			}

			TreeNode( const T& value, ull count ) : value( value ), one( nullptr ), zero( nullptr )
			{
				is_leaf = true;
				priority = count;
			}

			~TreeNode( )
			{
				delete one;
				delete zero;
			}

			bool stores_value( ) const
			{
				return is_leaf;
			}

			T get_value( ) const
			{
				return value;
			}

			TreeNode *get_one( ) const
			{
				return one;
			}

			TreeNode *get_zero( ) const
			{
				return zero;
			}

			friend struct less;

			struct less
			{
				bool operator() ( TreeNode<T> *a, TreeNode<T> *b )
				{
					return a->priority > b->priority;
				}
			};
		};

		template <typename T> class HuffmanTree
		{
		protected: typedef TreeNode<T> Node;

		private:
			void create_via_dfs( bit_set code, Node *node )
			{
				if ( node->stores_value( ) )
				{
					encoding[node->get_value( )] = code;
					return;
				}

				code.push_back( 1 );

				if ( node->get_one( ) != nullptr )
				{
					create_via_dfs( code, node->get_one( ) );
				}
				if ( node->get_zero( ) != nullptr )
				{
					code[code.size( ) - 1] = 0;
					create_via_dfs( code, node->get_zero( ) );
				}
			}

			void pack_tree( Node *node, IBinaryOutputStream &stream )
			{
				if ( node == nullptr )
					return;

				if ( node->stores_value( ) )
				{
					stream.write_bit( 0 );
					stream << node->get_value( );
					return;
				}

				stream.write_bit( 1 );
				pack_tree( node->get_zero( ), stream );
				pack_tree( node->get_one( ), stream );
			}

			static Node *unpack_tree( IBinaryInputStream &stream )
			{
				auto bit = stream.read_bit( );

				if ( bit ) // == 1
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

		protected:
			Node *root, *curr;
			std::map<T, bit_set> encoding;

			HuffmanTree( Node *tree_root ) : root( tree_root )
			{
				curr = root;
				if ( root != nullptr )
				{
					create_encoding( );
				}
			}

			void create_encoding( )
			{
				if ( root == nullptr )
					return;
				bit_set empty_code;
				create_via_dfs( empty_code, root );
			}

		public:
			static HuffmanTree deserialize( IBinaryInputStream &stream )
			{
				auto root = unpack_tree( stream );
				return HuffmanTree( root );
			}

			HuffmanTree( const HuffmanTree & ) = delete;

			HuffmanTree( HuffmanTree &&tree )
			{
				root = tree.root;
				curr = tree.curr;
				tree.curr = nullptr;
				tree.root = nullptr;
				encoding = std::move( tree.encoding );
			}

			HuffmanTree( const std::map<T, ull> &frequences )
			{
				priority_queue<Node *, vector<Node *>, Node::less> queue;

				for ( auto &pair : frequences )
				{
					if ( pair.second != 0UL )
					{
						queue.push( new Node( pair.first, pair.second ) );
					}
				}

				if ( queue.size( ) == 1 )
				{
					auto stub = new Node( std::numeric_limits<double>::infinity( ), 0ULL );
					root = new Node( stub, queue.top( ) );
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
					root = curr = queue.top( );
				}
				create_encoding( );
			}

			~HuffmanTree( )
			{
				delete root;
				encoding.clear( );
			}

			const bit_set &get_encoding( const T &symbol )
			{
				return encoding[symbol];
			}

			size_t get_encoding_length( const T &symbol )
			{
				return encoding[symbol].size( );
			}

			const T *make_step( bool direction )
			{
				const T *result = nullptr;

				if ( direction )
					curr = curr->get_one( );
				else
					curr = curr->get_zero( );

				if ( curr->stores_value( ) )
				{
					result = &( curr->value );
					curr = root;
				}

				return result;
			}

			void serialize( IBinaryOutputStream &stream )
			{
				pack_tree( root, stream );
			}
		};
	}
}

