#include "CorrelationCounter.h"

#include <cmath>

namespace quantpressor
{
	namespace column_sort
	{
		double CorrelationCounter::operator( )( const module_api::pIGrid &grid,
												module_api::uint first_column,
												module_api::uint second_column ) const
		{
			double EX1 = 0, EX2 = 0;

#pragma omp parallel for reduction( + : EX1, EX2 )
			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				EX1 += grid->get_value( i, first_column );
				EX2 += grid->get_value( i, second_column );
			}
			EX1 /= grid->get_row_count( );
			EX2 /= grid->get_row_count( );

			double DX1 = 0, DX2 = 0;
			double correlation = 0;

#pragma omp parallel for reduction( + : DX1, DX2, correlation )
			for ( int i = 0; i < grid->get_row_count( ); ++i )
			{
				double first_shift = grid->get_value( i, first_column ) - EX1;
				double second_shift = grid->get_value( i, second_column ) - EX2;

				correlation += first_shift * second_shift;

				DX1 += first_shift * first_shift;
				DX2 += second_shift * second_shift;
			}

			auto square = std::sqrt( DX1 * DX2 );	
			correlation = square != 0 ? correlation / square : 0;

			return correlation;
		}

		TwoDimensionalArray<double> CorrelationCounter::operator( )( const module_api::pIGrid & grid ) const
		{
			auto column_count = grid->get_column_count( );
			TwoDimensionalArray<double> matrix( column_count, column_count );

			for ( auto i = 0U; i < column_count; ++i )
			{
				for ( auto j = i; j < column_count; ++j )
				{
					matrix( i, j ) = matrix( j, i ) = this->operator( )( grid, i, j );
				}
			}

			return std::move( matrix );
		}
	}
}
