#include "RearrangedGrid.h"
#include "RearrangedMetadata.h"
#include <exception.h>

namespace quantpressor
{
	namespace column_sort
	{
		using namespace module_api;
		using std::vector;

		RearrangedGrid::RearrangedGrid( const pIGrid &origin, vector<uint> &&new_column_order ) : 
			order( std::move( new_column_order ) ),
			grid( origin )
		{
			metadata = make_heap_aware<RearrangedMetadata>( grid->get_metadata( ), order );
		}

		RearrangedGrid::~RearrangedGrid( )
		{
			
		}

		double RearrangedGrid::get_value( uint row, uint column ) const
		{
			return grid->get_value( row, order[column] );
		}

		DoubleArray RearrangedGrid::get_rect( uint min_row, uint min_column, uint row_count, uint column_count ) const
		{
			auto result = DoubleArray( row_count * column_count );

			for ( auto i = 0U; i < row_count; ++i )
			{
				for ( auto j = 0U; j < column_count; ++j )
				{
					result[i * column_count + j] = grid->get_value( i + min_row, order[j + min_column] );
				}
			}

			return std::move( result );
		}

		uint RearrangedGrid::get_column_count( ) const
		{
			return grid->get_column_count( );
		}

		uint RearrangedGrid::get_row_count( ) const
		{
			return grid->get_row_count( );
		}

		pIMetadata RearrangedGrid::get_metadata( ) const
		{
			return metadata;
		}
	}
}