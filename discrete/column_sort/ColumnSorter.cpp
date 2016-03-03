#include "CorrelationCounter.h"
#include "ColumnSorter.h"
#include "RearrangedGrid.h"

#include <exception.h>
#include <set>
#include <list>

namespace quantpressor
{
	namespace column_sort
	{
		using namespace module_api;
		using std::vector;
		using std::list;
		using std::set;

		typedef double( *func )( double );

		static func abs = std::fabs;

		module_api::pIGrid ColumnSorter::sort_columns( const module_api::pIGrid & grid )
		{
			auto column_count = grid->get_column_count( );

			CorrelationCounter counter;
			auto correlation_matrix = counter( grid );

			set<list<uint>> links;
			for ( int i = 0; i < column_count; ++i )
			{
				list<uint> single_column;
				single_column.push_back( i );
				links.insert( std::move( single_column ) );
			}

			while ( links.size( ) > 1 )
			{
				set<list<uint>>::iterator first, second;
				double max = 0.0;

				for ( auto i = links.begin( ); i != links.end( ); ++i )
				{
					for ( auto j = links.begin( ); j != links.end( ); ++j )
					{
						if ( i == j )
							continue;

						if ( abs( correlation_matrix( i->back( ), j->front( ) ) ) > max )
						{
							first = i;
							second = j;
							max = abs( correlation_matrix( i->back( ), j->front( ) ) );
						}

						if ( abs( correlation_matrix( i->front( ), j->back( ) ) ) > max )
						{
							first = j;
							second = i;
							max = abs( correlation_matrix( i->front( ), j->back( ) ) );
						}
					}
				}

				auto left = std::move( *first );
				auto right = std::move( *second );
				links.erase( first );
				links.erase( second );

				left.splice( left.end( ), std::move( right ) );
				links.insert( std::move( left ) );
			}
			auto result_list = std::move( *links.begin( ) );
			return make_heap_aware<RearrangedGrid>( grid, vector<uint>( result_list.begin( ), result_list.end( ) ) );
		}
	}
}