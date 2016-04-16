#include "ArithmeticCodingCompressor.h"

#include <exception.h>
#include <RoughGrid.h>
#include <cmath>
#include <algorithm>

#include "utilities.h"

namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
		using std::pair;
		template<typename K, typename V> using map = std::unordered_map<K, V>;

		static const int BIT_COUNT = 8;
		static const int REGISTER_SIZE = sizeof( ArithmeticCodingCompressor::register_type ) * ( BIT_COUNT / 2 );
		static const ArithmeticCodingCompressor::register_type FULL_REGISTER = ~( ArithmeticCodingCompressor::register_type( -1 ) << REGISTER_SIZE );

		// using 32 of 64 register bits; other will be reserved to prevent the overflow during multiplication
		inline bool most_significant_bit( ArithmeticCodingCompressor::register_type reg )
		{
			return ( reg >> ( REGISTER_SIZE - 1 ) ) % 2 == 1;
		}

		inline bool prev_most_significant_bit( ArithmeticCodingCompressor::register_type reg )
		{
			return ( ( reg << 1 ) >> ( REGISTER_SIZE - 1 ) ) % 2 == 1;
		}

		inline void set_most_significant_bit( ArithmeticCodingCompressor::register_type &reg, bool bit )
		{
			reg = bit ? ( reg | ( 1 << ( REGISTER_SIZE - 1 ) ) ) : ( reg & ( FULL_REGISTER >> 1 ) );
		}

		inline void cut_register( ArithmeticCodingCompressor::register_type &reg )
		{
			reg &= ~( FULL_REGISTER << REGISTER_SIZE );
		}

		inline void fill_register( IBinaryInputStream &stream, ArithmeticCodingCompressor::register_type &reg )
		{
			reg = 0;
			for ( int i = 0; i < REGISTER_SIZE; ++i )
			{
				reg <<= 1;
				reg += stream.read_bit( ) ? 1 : 0;
			}
		}

		map<double, ull> freqs_to_cumuls( const map<double, ull> &freqs )
		{
			auto sorted_freqs = vector<pair<double, ull>>( freqs.begin( ), freqs.end( ) );
			std::sort( sorted_freqs.begin( ), sorted_freqs.end( ), []( pair<double, ull> a, pair<double, ull> b )
			{
				return a.first < b.first;
			} );

			map<double, ull> cumuls;

			cumuls[sorted_freqs[0].first] = 0;
			for ( int i = 1; i < sorted_freqs.size( ); ++i )
			{
				cumuls[sorted_freqs[i].first] =
					cumuls[sorted_freqs[i - 1].first] +
					sorted_freqs[i - 1].second;
			}

			return std::move( cumuls );
		}

		///<summary>Removes codes with zero frequence from quantization</summary>
		Quantizations cut_quantizations( const Quantizations &quantizations, int column_count, const vector<map<double, ull>> &frequences )
		{
			Quantizations new_quantizations( column_count );

			for ( int column = 0; column < column_count; ++column )
			{
				vector<double> new_borders, new_codes;

				for ( int i = 0; i < quantizations[column].codes.size( ); ++i )
				{
					if ( frequences[column].find( quantizations[column].codes[i] ) != frequences[column].end( ) )
					{
						new_borders.push_back( quantizations[column].borders[i] );
						new_codes.push_back( quantizations[column].codes[i] );
					}
				}

				new_borders[0] = quantizations[column].borders[0];
				new_borders.push_back( quantizations[column].borders[quantizations[column].borders.size( ) - 1] );

				new_quantizations[column].borders = std::move( new_borders );
				new_quantizations[column].codes = std::move( new_codes );
				new_quantizations[column].deviation = quantizations[column].deviation;
				new_quantizations[column].entropy = quantizations[column].entropy;
			}

			return std::move( new_quantizations );
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

			// filling frequences table, counting errors
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

			Quantizations new_quantizations = cut_quantizations( quantizations,
																 grid->get_column_count( ),
																 frequences );

			vector<map<double, ull>> cumulative( grid->get_column_count( ) );

			for ( int column = 0; column < grid->get_column_count( ); ++column )
			{ cumulative[column] = freqs_to_cumuls( frequences[column] ); }

			serialize_frequences( frequences, stream );

			register_type low, high, delta, symbol_count;
			// assume that message is one column
			symbol_count = grid->get_row_count( );
			low = 0;
			high = FULL_REGISTER;
			int s = 0;

			auto start_pos = stream.get_current_position( );

			for ( int row = 0; row < grid->get_row_count( ); ++row )
			{
				for ( int column = 0; column < grid->get_column_count( ); ++column )
				{
					auto index = find_code_index( grid->get_value( row, column ), new_quantizations[column] );
					
					delta = high - low + 1;
					high = index != ( new_quantizations[column].codes.size( ) - 1 ) ?
						low + ( delta * cumulative[column][new_quantizations[column].codes[index + 1]] ) / symbol_count - 1
						: low + delta - 1;
					low = low + ( delta * cumulative[column][new_quantizations[column].codes[index]] ) / symbol_count;
					
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

					set_most_significant_bit( low, false );
					set_most_significant_bit( high, true );
					cut_register( low );
					cut_register( high );
				}
			}

			++s;
			stream.write_bit( prev_most_significant_bit( low ) );
			for ( int i = 0; i < s; ++i )
			{ stream.write_bit( !prev_most_significant_bit( low ) ); }

			auto bit_count = stream.get_current_position( ) - start_pos;
			auto bps = double( bit_count ) / ( grid->get_row_count( ) * grid->get_column_count( ) );

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
			
			auto cumulative = deserialize_cumulative( column_count, stream );

			register_type low, high, delta, current_bits, symbol_count = row_count;

			low = 0;
			high = FULL_REGISTER;
			fill_register( stream, current_bits );

			auto result = module_api::make_heap_aware<igor::RoughGrid>( row_count, column_count );

			for ( int i = 0; i < row_count * column_count; ++i )
			{
				delta = high - low + 1;
				auto z = ( symbol_count * ( current_bits - low + 1 ) - 1 ) / delta;
				
				double symbol;
				ull symbol_cumulative = 0, next_cumulative = symbol_count;

				for ( auto &pair : cumulative[i % column_count] )
				{
					if ( z >= pair.second && symbol_cumulative <= pair.second )
					{
						symbol_cumulative = pair.second;
						symbol = pair.first;
					}
					if ( z < pair.second && next_cumulative >= pair.second )
					{
						next_cumulative = pair.second;
					}
				}

				high = low + ( delta * next_cumulative ) / symbol_count - 1;
				low = low + ( delta * symbol_cumulative ) / symbol_count;

				while ( most_significant_bit( low ) == most_significant_bit( high ) )
				{
					current_bits <<= 1;
					current_bits += stream.read_bit( ) ? 1 : 0;
					low <<= 1;
					high = ( high << 1 ) + 1;
				}

				auto bit = most_significant_bit( current_bits );

				while ( prev_most_significant_bit( low ) && !prev_most_significant_bit( high ) )
				{
					current_bits <<= 1;
					current_bits += stream.read_bit( ) ? 1 : 0;
					low <<= 1;
					high = ( high << 1 ) + 1;
				}

				set_most_significant_bit( low, false );
				set_most_significant_bit( high, true );
				set_most_significant_bit( current_bits, bit );
				cut_register( low );
				cut_register( high );
				cut_register( current_bits );

				result->set_value( i / column_count, i % column_count, symbol );
			}

			return std::move( result );
		}

		void ArithmeticCodingCompressor::serialize_frequences( const std::vector<std::unordered_map<double, ull>>& frequences, IBinaryOutputStream & stream ) const
		{
			for ( auto &freq_map : frequences )
			{
				stream << freq_map.size( );
				
				ull max_q = 0;
				for ( auto &pair : freq_map )
				{
					if ( pair.second > max_q )
					{ max_q = pair.second; }
				}

				byte length = static_cast<byte>( std::log2( max_q ) ) + 1;
				stream << length;

				for ( auto &pair : freq_map )
				{
					stream << pair.first;

					for ( int i = length - 1; i >= 0; --i )
					{
						stream.write_bit( ( pair.second >> i ) % 2 );
					}
				}
			}
		}

		std::vector<std::unordered_map<double, ull>> ArithmeticCodingCompressor::deserialize_cumulative( module_api::uint column_count,
																										 IBinaryInputStream & stream ) const
		{
			vector<map<double, ull>> result;

			for ( int i = 0; i < column_count; ++i )
			{
				if ( i == 3 )
				{
					int x = 0;
				}

				size_t map_size;
				stream >> map_size;

				byte length;
				stream >> length;

				map<double, ull> freqs;
				double key;

				for ( int j = 0; j < map_size; ++j )
				{
					stream >> key;

					ull value = 0;

					for ( int i = 0; i < length; ++i )
					{
						value <<= 1;
						value += stream.read_bit( ) ? 1 : 0;
					}

					freqs[key] = value;
				}

				result.push_back( freqs_to_cumuls( freqs ) );
			}

			return std::move( result );
		}
	}
}
