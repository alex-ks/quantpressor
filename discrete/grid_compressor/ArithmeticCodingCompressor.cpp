#include "ArithmeticCodingCompressor.h"

#include <exception.h>
#include <cmath>

#include "utilities.h"

namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
		template<typename K, typename V> using map = std::unordered_map<K, V>;

		static const int BIT_COUNT = 8;

		inline bool most_significant_bit( ArithmeticCodingCompressor::register_type reg )
		{
			return reg >> ( sizeof( reg ) * BIT_COUNT - 1 ) == 1;
		}

		inline bool prev_most_significant_bit( ArithmeticCodingCompressor::register_type reg )
		{
			return ( reg << 1 ) >> ( sizeof( reg ) * BIT_COUNT - 1 ) == 1;
		}

		CompressionResult ArithmeticCodingCompressor::compress( const module_api::pIGrid &grid,
																const Quantizations &quantizations, 
																IBinaryOutputStream &stream ) const
		{
			stream << grid->get_row_count( );
			stream << grid->get_column_count( );

			CompressionResult result;
			vector<map<double, ull>> frequences( grid->get_column_count( ) );
			vector<double> 
				dxs( grid->get_column_count( ) ),
				min_err( grid->get_column_count( ) ), 
				avg_err( grid->get_column_count( ) ), 
				max_err( grid->get_column_count( ) );

			for ( int column = 0; column < grid->get_column_count( ); ++column )
			{
				min_err[column] = std::numeric_limits<double>::max( );
				max_err[column] = std::numeric_limits<double>::min( );
				avg_err[column] = 0.0;
				dxs[column] = 0.0;

				for ( int row = 0; row < grid->get_row_count( ); ++row )
				{
					auto value = grid->get_value( row, column );
					auto code = find_code( grid->get_value( row, column ), quantizations[column] );
					auto delta = std::fabs( value - code );

					++( frequences[column][code] );
					
					if ( min_err[column] > delta )
					{ min_err[column] = delta; }
					if ( max_err[column] < delta )
					{ max_err[column] = delta; }
					avg_err[column] = row > 0 ? ( avg_err[column] * row + delta ) / ( row + 1 ) : delta;
					dxs[column] = row > 0 ? ( dxs[column] * row + delta * delta ) / ( row + 1 ) : delta * delta;
				}
			}

			result.avg_errors = std::move( avg_err );
			result.min_errors = std::move( min_err );
			result.max_errors = std::move( max_err );
			result.real_variances = std::move( dxs );

			vector<map<double, ull>> qumulative( grid->get_column_count( ) );

			for ( int column = 0; column < grid->get_column_count( ); ++column )
			{
				qumulative[column][quantizations[column].codes[0]] = 0;

				for ( int i = 1; i < quantizations[column].codes.size( ); ++i )
				{
					qumulative[column][quantizations[column].codes[i]] =
						qumulative[column][quantizations[column].codes[i - 1]] +
						frequences[column][quantizations[column].codes[i - 1]];
				}
			}

			serialize_frequences( qumulative, stream );

			register_type low, high, delta, symbol_count;
			// assume that message is one column
			symbol_count = grid->get_row_count( );
			low = 0;
			high = register_type( -1 ) - 1;
			int s = 0;

			auto start_pos = stream.get_current_position( );

			for ( int row = 0; row < grid->get_row_count( ); ++row )
			{
				for ( int column = 0; column < grid->get_column_count( ); ++column )
				{
					auto index = find_code_index( grid->get_value( row, column ), quantizations[column] );
					
					delta = high - low + 1;
					high = index != ( quantizations[column].codes.size( ) - 1 ) ?
						low + ( delta * qumulative[column][quantizations[column].codes[index + 1]] ) / symbol_count - 1
						: low + delta - 1;
					low = low + ( delta * qumulative[column][quantizations[column].codes[index]] ) / symbol_count;
					
					while ( most_significant_bit( low ) == most_significant_bit( high ) )
					{
						stream.write_bit( most_significant_bit( low ) );
						for ( int i = 0; i < s; ++i )
						{ stream.write_bit( !most_significant_bit( low ) ); }
						s = 0;
						low <<= 1;
						high = ( high << 1 ) + 1;
					}

					while ( prev_most_significant_bit( low ) && !prev_most_significant_bit( high ) )
					{
						++s;
						low <<= 1;
						high = ( high << 1 ) + 1;
					}

					low = low & ( register_type( -1 ) >> 1 );
					high = high | ( register_type( 1 ) << ( sizeof( register_type ) * BIT_COUNT - 1 ) );
				}
			}

			++s;
			stream.write_bit( prev_most_significant_bit( low ) );
			for ( int i = 0; i < s; ++i )
			{ stream.write_bit( !prev_most_significant_bit( low ) ); }

			auto bit_count = stream.get_current_position( ) - start_pos;
			auto bps = double( bit_count ) / ( symbol_count + 1 );

			for ( int i = 0; i < grid->get_column_count( ); ++i )
			{
				result.columns_bps.push_back( bps );
			}

			result.extra_results[L"Data size"] = std::to_wstring( double( bit_count ) / BIT_COUNT / 1024 ) + L" KB";

			return std::move( result );
		}

		module_api::pIGrid ArithmeticCodingCompressor::decompress( IBinaryInputStream &stream ) const
		{
			module_api::uint row_count, column_count;
			stream >> row_count;
			stream >> column_count;
			
			auto qumulative = deserialize_frequences( column_count, stream );

			register_type low, high, delta, current_bits, symbol_count = row_count;

			low = 0;
			high = register_type( -1 ) - 1;
			stream >> current_bits;

			throw module_api::NotImplementedException( );
		}

		void ArithmeticCodingCompressor::serialize_frequences( const std::vector<std::unordered_map<double, ull>>& frequences, IBinaryOutputStream & stream ) const
		{
			for ( auto &freq_map : frequences )
			{
				stream << freq_map.size( );
				
				for ( auto &pair : freq_map )
				{
					stream << pair.first;
					stream << pair.second;
				}
			}
		}

		std::vector<std::unordered_map<double, ull>> ArithmeticCodingCompressor::deserialize_frequences( module_api::uint column_count,
																										 IBinaryInputStream & stream ) const
		{
			vector<map<double, ull>> result;

			for ( int i = 0; i < column_count; ++i )
			{
				size_t map_size;
				stream >> map_size;

				map<double, ull> freqs;
				double key;
				ull value;

				for ( int j = 0; j < map_size; ++j )
				{
					stream >> key;
					stream >> value;
					freqs[key] = value;
				}

				result.push_back( std::move( freqs ) );
			}

			return std::move( result );
		}
	}
}
