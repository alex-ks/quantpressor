#include "ModularArithmeticCompressor.h"
#include <exception.h>

#include <map>
#include <vector>

#include "utilities.h"

using namespace grid_compressor;

using std::map;
using std::vector;

namespace arithmetic_coding
{
	double ModularArithmeticCompressor::compress_column( module_api::uint column,
														 const module_api::pIGrid &grid,
														 const grid_compressor::Quantization &quantization,
														 const grid_compressor::IBinaryOutputStream &s )
	{
		map<double, ull> counts;
		auto &stream = const_cast< IBinaryOutputStream & >( s );

		for ( double code : quantization.codes )
		{
			counts[code] = 0UL;
		}

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{
			++counts[find_code( grid->get_value( i, column ), quantization )];
		}

		map<double, double> cumulative_probabilities;
		double last_probability = 0.0;

		for ( int i = 0; i < quantization.codes.size( ); ++i )
		{
			double probability = static_cast< double >( counts[quantization.codes[i]] ) / grid->get_row_count( );
			cumulative_probabilities[quantization.codes[i]] = i > 0 ?
				( cumulative_probabilities[quantization.codes[i - 1]] + last_probability ) : 0;
			last_probability = probability;
		}

		BlockIndexType block_index = 0;

		double left_border = 0.0, delta = 1.0;

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{
			left_border = left_border + delta * cumulative_probabilities[find_code( grid->get_value( i, column ), quantization )];
		}

		throw module_api::NotImplementedException( );
	}

	ModularArithmeticCompressor::ModularArithmeticCompressor( ) : ModularArithmeticCompressor( DEFAULT_BLOCK_SIZE )
	{

	}

	ModularArithmeticCompressor::ModularArithmeticCompressor( BlockIndexType block_size ) : block_size( block_size )
	{

	}

	ModularArithmeticCompressor::~ModularArithmeticCompressor( )
	{

	}

	vector<double> ModularArithmeticCompressor::compress( const module_api::pIGrid &grid,
														  const grid_compressor::Quantizations &quantizations,
														  const grid_compressor::IBinaryOutputStream &s )
	{
		throw module_api::NotImplementedException( );
	}

	module_api::pIGrid ModularArithmeticCompressor::decompress( const grid_compressor::IBinaryInputStream &stream )
	{
		throw module_api::NotImplementedException( );
	}
}
