#pragma once

#include <io.h>
#include <vector>

namespace distributions
{
	class SampleExtractor
	{
	private:
		int max_size;

	public:
		SampleExtractor( int max_sample_size );
		SampleExtractor( );

		std::vector<double> extract_sample( module_api::uint column, const module_api::pIGrid &grid ) const;

		static const int DEFAULT_MAX_SAMPLE_SIZE = 1000;
	};
}
