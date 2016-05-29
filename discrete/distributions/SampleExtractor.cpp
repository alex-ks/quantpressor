#include "SampleExtractor.h"

#include <random>

namespace distributions
{
	using std::vector;
	using module_api::uint;
	using module_api::pIGrid;

	SampleExtractor::SampleExtractor( int max_sample_size )
	{
		max_size = max_sample_size;
	}

	SampleExtractor::SampleExtractor( ) : SampleExtractor( DEFAULT_MAX_SAMPLE_SIZE )
	{

	}

	vector<double> extract_full_sample( uint column, const pIGrid &grid )
	{
		vector<double> sample( grid->get_row_count( ) );

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{ sample[i] = grid->get_value( i, column ); }

		return std::move( sample );
	}

	vector<double> SampleExtractor::extract_sample( uint column, const pIGrid & grid ) const
	{
		auto count = grid->get_row_count( );

		if ( count <= max_size )
		{ return extract_full_sample( column, grid ); }
		
		//generates a sequence of uniformly distributed random numbers
		std::random_device random;
		
		vector<int> indexes( count );

		for ( int i = 0; i < count; ++i )
		{ indexes[i] = i; }

		vector<double> result( max_size );

		for ( int i = 0; i < max_size; ++i )
		{
			int index = indexes[random( ) % ( count - i )];
			std::swap( indexes[index], indexes[count - i - 1] );
			result[i] = grid->get_value( index, column );
		}

		return std::move( result );
	}
}
