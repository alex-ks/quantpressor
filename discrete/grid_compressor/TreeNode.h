#pragma once

namespace quantpressor
{
	namespace compressors
	{
		typedef unsigned long long ull;

		template <typename T> class TreeNode
		{
		private:
			T value;
			ull priority;
			bool is_leaf;
			TreeNode *one, *zero;

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
	}
}

