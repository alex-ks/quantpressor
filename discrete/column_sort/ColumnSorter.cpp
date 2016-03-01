#include "CorrelationCounter.h"
#include "ColumnSorter.h"
#include "RearrangedGrid.h"

#include <exception.h>

namespace quantpressor
{
	namespace column_sort
	{
		using namespace module_api;
		using std::vector;

		typedef double( *func )( double );

		static func abs = std::fabs;

		struct ColumnLink
		{
			int left, right;

			ColumnLink( ) : left( -1 ), right( -1 )
			{

			}
		};

		module_api::pIGrid ColumnSorter::sort_columns( const module_api::pIGrid & grid )
		{
			auto column_count = grid->get_column_count( );

			vector<bool> full( column_count );
			std::fill( full.begin( ), full.end( ), false );

			vector<ColumnLink> links( column_count );

			CorrelationCounter counter;
			auto correlation_matrix = counter( grid );

			uint full_count = 0;

			while ( full_count != column_count )
			{
				uint first_column, second_column;
				double max_corr = 0.0;

				for ( auto i = 0U; i < column_count; ++i )
				{
					for ( auto j = i; j < column_count; ++j )
					{
						if ( !full[i] && !full[j] && abs( correlation_matrix( i, j ) ) > max_corr
							 && ( ( links[i].left == -1 && links[j].right == -1 ) || ( links[i].right == -1 && links[j].left == -1 ) ) )
						{
							first_column = i;
							second_column = j;
							max_corr = abs( correlation_matrix( i, j ) );
						}
					}
				}

				correlation_matrix( first_column, second_column ) = 
					correlation_matrix( first_column, second_column ) = 0.0;

				if ( links[first_column].left == -1 )
				{
					links[first_column].left = second_column;
					links[second_column].right = first_column;
				}
				else
				{
					links[first_column].right = second_column;
					links[second_column].left = first_column;
				}

				if ( links[first_column].left != -1 && links[first_column].right != -1 )
				{
					full[first_column] = true;
					++full_count;
				}

				if ( links[second_column].left != -1 && links[second_column].right != -1 )
				{
					full[second_column] = true;
					++full_count;
				}
			}

			uint curr;

			for ( auto i = 0U; i < links.size( ); ++i )
			{
				if ( links[i].left == -1 )
				{
					curr = i;
					break;
				}
			}

			vector<uint> new_order;

			while ( curr != -1 )
			{
				new_order.push_back( curr );
				curr = links[curr].right;
			}

			return make_heap_aware<RearrangedGrid>( grid, std::move( new_order ) );
		}
	}
}