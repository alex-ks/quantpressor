#include "ArithmeticCodingCompressor.h"

#include <exception.h>
#include <RoughGrid.h>
#include <cmath>

#include "utilities.h"

namespace quantpressor
{
	namespace compressors
	{
		using std::vector;
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
			//high = register_type( -1 ) - 1;
			high = FULL_REGISTER;
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
			
			auto qumulative = deserialize_frequences( column_count, stream );

			register_type low, high, delta, current_bits, symbol_count = row_count;

			low = 0;
			//high = register_type( -1 ) - 1;
			high = FULL_REGISTER;
			fill_register( stream, current_bits );

			auto result = module_api::make_heap_aware<igor::RoughGrid>( row_count, column_count );

			for ( int i = 0; i < row_count * column_count; ++i )
			{
				delta = high - low + 1;
				auto z = ( symbol_count * ( current_bits - low + 1 ) - 1 ) / delta;
				
				double symbol;
				ull symbol_qumulative = 0, next_cumulative = symbol_count;

				for ( auto &pair : qumulative[i % column_count] )
				{
					if ( z >= pair.second && symbol_qumulative <= pair.second )
					{
						symbol_qumulative = pair.second;
						symbol = pair.first;
					}
					if ( z < pair.second && next_cumulative >= pair.second )
					{
						next_cumulative = pair.second;
					}
				}

				high = low + ( delta * next_cumulative ) / symbol_count - 1;
				low = low + ( delta * symbol_qumulative ) / symbol_count;

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

				byte length = std::ceil( std::log2( max_q ) );
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

		std::vector<std::unordered_map<double, ull>> ArithmeticCodingCompressor::deserialize_frequences( module_api::uint column_count,
																										 IBinaryInputStream & stream ) const
		{
			vector<map<double, ull>> result;

			for ( int i = 0; i < column_count; ++i )
			{
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

				result.push_back( std::move( freqs ) );
			}

			return std::move( result );
		}
	}
}
