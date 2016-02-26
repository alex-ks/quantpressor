#pragma once
#include <map>
#include <queue>
#include <vector>
#include <exception.h>

#include "IBinaryInputStream.h"
#include "IBinaryOutputStream.h"

namespace quantpressor
{
	namespace compressors
	{
		typedef std::vector<bool> bit_set;

		namespace huffman
		{
			template <typename T> class HuffmanTree;
			template <typename T> class DynamicHuffmanTree;

			template <typename T> class TreeNode
			{
			private:
				T value;
				ull priority;
				bool is_leaf;
				TreeNode *one, *zero;

				friend HuffmanTree<T>;
				friend DynamicHuffmanTree<T>;

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

				struct less;

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
			protected:
				typedef TreeNode<T> Node;

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
						T value;
						stream >> value;
						return new Node( value, 0ULL );
					}
				}

			public:
				static HuffmanTree deserialize( IBinaryInputStream &stream )
				{
					auto root = unpack_tree( stream );
					return HuffmanTree( root );
				}

				HuffmanTree( const HuffmanTree & ) = delete;
				HuffmanTree &operator=( const HuffmanTree & ) = delete;

				HuffmanTree( HuffmanTree &&tree )
				{
					root = tree.root;
					curr = tree.curr;
					tree.curr = nullptr;
					tree.root = nullptr;
					encoding = std::move( tree.encoding );
				}

				HuffmanTree &operator=( HuffmanTree &&tree )
				{
					if ( &tree != this )
					{
						root = tree.root;
						curr = tree.curr;
						tree.curr = nullptr;
						tree.root = nullptr;
						encoding = std::move( tree.encoding );
					}
					return *this;
				}

				HuffmanTree( const std::map<T, ull> &frequences )
				{
					std::priority_queue<Node *, std::vector<Node *>, Node::less> queue;

					for ( auto &pair : frequences )
					{
						if ( pair.second != 0UL )
						{
							queue.push( new Node( pair.first, pair.second ) );
						}
					}

					if ( queue.size( ) == 1 )
					{
						auto stub = new Node( T( ), 0ULL );
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

				virtual ~HuffmanTree( )
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

			template <typename T> class DynamicHuffmanTree : public HuffmanTree<T>
			{
			private:
				Node *empty;
				std::vector<Node *> sublings;
				bool need_reencode = false;

				void build_sublings( Node *node )
				{
					if ( node == nullptr )
						return;

					Node::less more;

					if ( more( node->one, node->zero ) )
					{
						sublings.push_back( node->one );
						sublings.push_back( node->zero );
					}
					else
					{
						sublings.push_back( node->zero );
						sublings.push_back( node->one );
					}

					if ( !node->one->is_leaf )
						build_sublings( node->one );

					if ( !node->zero->is_leaf )
						build_sublings( node->zero );
				}

				void recalc_priority( Node *node )
				{
					if ( node->is_leaf )
						return;

					recalc_priority( node->one );
					recalc_priority( node->zero );

					node->priority = node->one->priority + node->zero->priority;
				}

				void rebuild_tree( int start )
				{
					Node::less more;
					int last_less = start;

					for ( int i = start - 1; i > 0; --i )
					{
						last_less = more( sublings[start], sublings[i] ) ? i : last_less;
					}

					if ( last_less == start )
					{
						return;
					}

					need_reencode = true;
					std::swap( *sublings[start], *sublings[last_less] );
					recalc_priority( root );

					if ( last_less != 0 )
					{
						rebuild_tree( last_less );
					}
				}

				Node *find_by_value( Node *node, const T &value )
				{
					if ( node == nullptr )
						return nullptr;

					if ( node->is_leaf )
					{
						return node->value == value ? node : nullptr;
					}

					auto suspect = find_by_value( node->one, value );

					if ( suspect != nullptr )
					{
						return suspect;
					}
					else
					{
						return find_by_value( node->zero, value );
					}
				}

				void assume_frequence( Node *node, const std::map<T, ull> &frequences )
				{
					if ( node == nullptr )
						return;

					if ( node->is_leaf && node != empty )
					{
						node->priority = frequences.at( node->value );
					}
					else
					{
						assume_frequence( node->one, frequences );
						assume_frequence( node->zero, frequences );
					}
				}

			public:
				static DynamicHuffmanTree deserialize( IBinaryInputStream &stream, const T &empty_symbol )
				{
					auto root = unpack_tree( stream );
					return DynamicHuffmanTree( root, empty_symbol );
				}

				DynamicHuffmanTree( const DynamicHuffmanTree & ) = delete;
				DynamicHuffmanTree &operator=( const DynamicHuffmanTree & ) = delete;

				DynamicHuffmanTree( DynamicHuffmanTree &&tree ) : HuffmanTree( std::move( tree ) )
				{
					empty = tree.empty;
					tree.empty = nullptr;
					sublings = std::move( tree.sublings );
				}

				DynamicHuffmanTree &operator=( DynamicHuffmanTree &&tree )
				{
					if ( &tree != this )
					{
						this->operator=( tree );
						empty = tree.empty;
						tree.empty = nullptr;
						sublings = std::move( tree.sublings );
					}
				}

				DynamicHuffmanTree( Node *root, const T &empty_symbol ) : HuffmanTree( root )
				{
					empty = find_by_value( root, empty_symbol );
					if ( empty == nullptr )
						throw module_api::BadInputException( L"Empty symbol is not found" );
					empty->priority = 0ULL;
					sublings.push_back( root );
					build_sublings( root );
				}

				DynamicHuffmanTree( const std::map<T, ull> &start_frequences, T empty_symbol ) : HuffmanTree( nullptr )
				{
					std::priority_queue<Node *, std::vector<Node *>, Node::less> queue;

					for ( auto &pair : start_frequences )
					{
						if ( pair.second != 0UL )
						{
							queue.push( new Node( pair.first, pair.second ) );
						}
					}

					empty = new Node( empty_symbol, 0ULL );

					if ( queue.size( ) != 0 )
					{
						queue.push( empty );

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
					else
					{
						root = empty;
						sublings.push_back( root );
					}

					create_encoding( );
					sublings.push_back( root );
					build_sublings( root );
				}

				bool add_new_symbol( const T& symbol )
				{
					auto it = encoding.find( symbol );

					if ( it == encoding.end( ) )
					{
						empty->is_leaf = false;
						empty->one = new Node( symbol, 1ULL );
						empty->zero = new Node( empty->value, 0ULL );

						auto path = encoding[empty->value];
						path.push_back( true );
						encoding[symbol] = std::move( path );
						encoding[empty->value].push_back( false );

						sublings.push_back( empty->one );
						sublings.push_back( empty->zero );

						empty = empty->zero;
						recalc_priority( root );
						return true;
					}
					else
					{
						auto &path = it->second;

						// increment priority of found symbol and all its ancestors
						Node *ptr = root;
						for ( auto &dir : path )
						{
							++( ptr->priority );
							if ( dir )
							{
								ptr = ptr->one;
							}
							else
							{
								ptr = ptr->zero;
							}
						}
						++( ptr->priority );

						// find index of changed node in sublings list
						int start = sublings.size( ) - 1;
						while ( sublings[start] != ptr )
						{
							--start;
						}
						rebuild_tree( start );

						if ( need_reencode )
						{
							create_encoding( );
							need_reencode = false;
						}
						return false;
					}
				}

				void assume_frequences( const std::map<T, ull> &frequences )
				{
					assume_frequence( root, frequences );
					recalc_priority( root );
				}
			};
		}
	}
}

