#pragma once

#include <cstdlib>

namespace quantpressor
{
	template <typename T> class TwoDimensionalArray
	{
	private:
		T *array;
		size_t column_count, row_count;

	public:
		TwoDimensionalArray( size_t row_count, size_t column_count ) noexcept
		{
			this->row_count = row_count;
			this->column_count = column_count;
			array = ( T * )std::malloc( row_count * column_count * sizeof( T ) );
		}

		TwoDimensionalArray( const TwoDimensionalArray & ) = delete;
		TwoDimensionalArray &operator= ( const TwoDimensionalArray & ) = delete;

		TwoDimensionalArray( TwoDimensionalArray &&two_array )
		{
			row_count = two_array.row_count;
			column_count = two_array.column_count;
			array = two_array.array;
			two_array.array = nullptr;
		}

		TwoDimensionalArray &operator=( TwoDimensionalArray &&two_array )
		{
			if ( &two_array != this )
			{
				std::free( array );
				row_count = two_array.row_count;
				column_count = two_array.column_count;
				array = two_array.array;
				two_array.array = nullptr;
			}
			return *this;
		}

		inline T &operator( ) ( size_t row, size_t column ) noexcept
		{
			return *( array + ( row * column_count + column ) );
		}

		inline size_t get_row_count( ) const noexcept
		{
			return row_count;
		}

		inline size_t get_column_count( ) const noexcept
		{
			return column_count;
		}

		inline size_t get_total_count( ) const noexcept
		{
			return row_count * column_count;
		}

		~TwoDimensionalArray( ) noexcept
		{
			std::free( array );
		}
	};
}