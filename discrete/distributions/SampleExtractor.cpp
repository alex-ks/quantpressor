#include "SampleExtractor.h"

namespace distributions
{
	using std::vector;

	SampleExtractor::SampleExtractor( int max_sample_size )
	{
		max_size = max_sample_size;
	}

	SampleExtractor::SampleExtractor( ) : SampleExtractor( DEFAULT_MAX_SAMPLE_SIZE )
	{

	}

	vector<double> SampleExtractor::extract_sample( module_api::uint column, const module_api::pIGrid & grid ) const
	{
		auto count = grid->get_row_count( );
		unsigned int step = 1;

		if ( count > max_size )
		{
			step = count / max_size;
			count = max_size;
		}

		vector<double> result( count );

		for ( int i = 0; i < count; ++i )
		{
			result[i] = grid->get_value( i * step, column );
		}

		return std::move( result );
	}
}
