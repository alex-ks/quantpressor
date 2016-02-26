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
			NativeCompressionResultCopy( const quantpressor::CompressionResult &compResult )
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

		HuffmanCompressor::HuffmanCompressor( )
		{
			nativeCompressor = new quantpressor::compressors::HuffmanCompressor( );
		}

		HuffmanCompressor::!HuffmanCompressor( )
		{
			if ( nativeCompressor != nullptr )
			{
				delete nativeCompressor;
				nativeCompressor = nullptr;
			}
		}

#pragma unmanaged

#include <stdio.h>

		quantpressor::CompressionResult CompressGrid( quantpressor::ICompressor &compressor,
													  const module_api::pIGrid &grid,
													  quantpressor::Quantizations &qs,
													  quantpressor::IBinaryOutputStream &stream )
		{
			/*freopen( "out.txt", "w", stdout );
			for ( auto &q : qs )
			{
				for ( auto &b : q.borders )
					printf( "%lf ", b );
				printf( "\n" );

				for ( auto &b : q.codes )
					printf( "%lf ", b );
				printf( "\n" );
			}
			fflush( stdout );*/
			return compressor.compress( grid, qs, stream );
		}

#pragma managed

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

			auto &nativeStream = *streamWrapper->NativePtr( );

			quantpressor::Quantizations qs;

			for each ( auto q in quantizations )
			{
				auto qWrapper = dynamic_cast< NativeQuantization ^ >( q );
				if ( qWrapper == nullptr )
				{
					auto Borders = q->Borders;
					auto Codes = q->Codes;

					quantpressor::Quantization quant( Borders->Length );
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

			auto params = CompressGrid( *nativeCompressor, nativeGrid, qs, nativeStream );

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