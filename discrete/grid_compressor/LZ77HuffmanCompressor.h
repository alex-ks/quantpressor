#pragma once
#include "ICompressor.h"

#include <deque>

namespace quantpressor
{
	namespace compressors
	{
		template <typename T> class Window;

		class LZ77HuffmanCompressor : public ICompressor
		{
			size_t width;

		public:
			LZ77HuffmanCompressor( size_t window_width );

			virtual CompressionResult compress( const module_api::pIGrid & grid, const Quantizations & quantizations, const IBinaryOutputStream & stream ) override;

			virtual module_api::pIGrid decompress( const IBinaryInputStream & stream ) override;

		};

		template <typename T> class Window : public std::deque<T>
		{
		private:
			size_t window_width;

		public:
			Window( size_t width ) : window_width( width )
			{
				
			}

			void push_back( const T &value )
			{
				if ( window_width == size( ) )
				{
					pop_front( );
				}
				std::deque<T>::push_back( value );
			}

			void push_back( T &&value )
			{
				if ( window_width == size( ) )
				{
					pop_front( );
				}
				std::deque<T>::push_back( std::move( value ) );
			}

			void push_front( const T &value )
			{
				if ( window_width == size( ) )
				{
					pop_back( );
				}
				std::deque<T>::push_front( value );
			}

			void push_front( T &&value )
			{
				if ( window_width == size( ) )
				{
					pop_back( );
				}
				std::deque<T>::push_front( std::move( value ) );
			}
		};
	}
}
