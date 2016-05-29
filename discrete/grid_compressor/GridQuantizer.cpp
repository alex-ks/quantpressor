#include "GridQuantizer.h"
#include "Quantizer.h"
#include <FastEmpiricalDistribution.h>
#include <SampleExtractor.h>
#include <exception.h>

namespace quantpressor
{
	using std::vector;
	using std::numeric_limits;
	using namespace distributions;

	typedef module_api::HeapPtr<distributions::IDistribution> pIDistribution;

	using module_api::make_heap_aware;

	vector<Quantization> GridQuantizer::quantize_grid( const module_api::pIGrid &grid, 
													   const std::vector<double> &max_errors ) const
	{
		auto count = grid->get_column_count( );

		if ( count != max_errors.size( ) )
		{ throw module_api::BadInputException( ); }

		vector<Quantization> qs( count );
		vector<pIDistribution> distrs( count );

#pragma omp parallel for schedule( dynamic )
		for ( int column = 0; column < count; ++column )
		{
			double left = numeric_limits<double>::max( );
			double right = numeric_limits<double>::min( );

			for ( int row = 0; row < grid->get_row_count( ); ++row )
			{
				auto value = grid->get_value( row, column );
				left = left < value ? left : value;
				right = right > value ? right : value;
			}

			left -= max_errors[column];
			right += max_errors[column];

			SampleExtractor extractor;
			auto sample = extractor.extract_sample( column, grid );
			auto method = DetailedApproximationMethod::gauss_kernel( sample );
			Quantizer quantizer( left, right );

			pIDistribution empirical = make_heap_aware<FastEmpiricalDistribution>( std::move( sample ), method );
			distrs[column] = empirical;

			qs[column] = quantizer.quantize( max_errors[column], *empirical );
		}

		return std::move( qs );
	}
}
