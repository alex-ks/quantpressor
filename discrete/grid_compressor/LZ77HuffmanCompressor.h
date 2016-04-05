#pragma once
#include "ICompressor.h"
#include "TripletHandlers.h"

#include <deque>

namespace quantpressor
{
	namespace compressors
	{
		template <typename T> class Window;

		class LZ77HuffmanCompressor : public ICompressor
		{
			width_t width;

		public:
			LZ77HuffmanCompressor( width_t window_width );

			virtual CompressionResult compress( const module_api::pIGrid &grid, 
												const Quantizations & quantizations, 
												IBinaryOutputStream &stream ) const override;

			virtual module_api::pIGrid decompress( IBinaryInputStream &stream ) const override;
		};

		template <typename T> class Window : public std::deque<T>
		{
		private:
			width_t window_width;

		public:
			Window( width_t width ) : window_width( width )
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
