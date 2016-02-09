#include "HuffmanCompressor.h"

#include "NativeGrid.h"
#include "FileInputStream.h"
#include "FileOutputStream.h"
#include "NativeQuantization.h"

namespace Quantpressor
{
	namespace Compressors
	{
		array<double> ^VectorToArray( const std::vector<double> &vector )
		{
			auto result = gcnew array<double>( vector.size( ) );
			for ( int i = 0; i < vector.size( ); ++i )
			{
				result[i] = vector[i];
			}
			return result;
		}

		ref class NativeCompressionResultCopy : public ICompressionResult
		{
		private:
			array<double> ^bpss, ^realVariances, ^min, ^max, ^avg;

		internal:
			NativeCompressionResultCopy( const grid_compressor::CompressionResult &compResult )
			{
				bpss = VectorToArray( compResult.columns_bps );
				realVariances = VectorToArray( compResult.real_variances );
				min = VectorToArray( compResult.min_errors );
				max = VectorToArray( compResult.max_errors );
				avg = VectorToArray( compResult.avg_errors );
			}

		public:
			virtual property array<double>^ ColumnsBitsPerSymbol
			{
				array<double> ^get( ) { return bpss; }
			}

			virtual property array<double>^ RealVariances
			{
				array<double> ^get( ) { return realVariances; }
			}

			virtual property array<double>^ MinErrors
			{
				array<double> ^get( ) { return min; }
			}

			virtual property array<double>^ AvgErrors
			{
				array<double> ^get( ) { return avg; }
			}

			virtual property array<double>^ MaxErrors
			{
				array<double> ^get( ) { return max; }
			}
		};

		ICompressionResult ^HuffmanCompressor::Compress( IGrid ^ grid,
														 System::Collections::Generic::IList<IQuantization^>^ quantizations,
														 IBinaryOutputStream ^ stream )
		{
			auto wrapper = dynamic_cast< NativeGrid ^ >( grid );
			if ( wrapper == nullptr )
				throw gcnew System::ArgumentException( L"Cannot work with managed grids" );

			auto nativeGrid = wrapper->NativePtr( );

			auto streamWrapper = dynamic_cast< FileIO::FileOutputStream ^ >( stream );
			if ( streamWrapper == nullptr )
				throw gcnew System::ArgumentException( L"Cannot work with managed binary streams" );

			auto nativeStream = streamWrapper->NativePtr( );

			grid_compressor::Quantizations qs;

			for each ( auto q in quantizations )
			{
				auto qWrapper = dynamic_cast< NativeQuantization ^ >( q );
				if ( qWrapper == nullptr )
				{
					auto Borders = q->Borders;
					auto Codes = q->Codes;

					grid_compressor::Quantization quant( Borders->Length );
					for ( int i = 0; i < Borders->Length; ++i )
					{
						quant.borders[i] = Borders[i];
						if ( i < Codes->Length )
						{
							quant.codes[i] = Codes[i];
						}
					}
					quant.deviation = q->Variance;
					quant.entropy = q->Entropy;
					qs.push_back( std::move( quant ) );
				}
				else
				{
					qs.push_back( qWrapper->NativeStruct( ) );
				}
			}

			auto params = nativeCompressor->compress( nativeGrid, qs, *nativeStream );

			return gcnew NativeCompressionResultCopy( params );
		}

		IGrid ^ HuffmanCompressor::Decompress( IBinaryInputStream ^ stream )
		{
			auto streamWrapper = dynamic_cast< FileIO::FileInputStream ^ >( stream );
			if ( streamWrapper == nullptr )
				throw gcnew System::ArgumentException( L"Cannot work with managed binary streams" );

			auto nativeStream = streamWrapper->NativePtr( );

			return gcnew NativeGrid( nativeCompressor->decompress( *nativeStream ) );
		}
	}
}