#include <vector>
#include <iostream>
#include <fstream>

#include <lib.h>
#include <Quantizer.h>
#include <HuffmanCompressor.h>
#include <NormalDistribution.h>
#include <ExponentialDistribution.h>

#include <FileInputStream.h>
#include <FileOutputStream.h>

#include "Timer.h"

using namespace std;
using namespace module_api;
using namespace distributions;
using namespace grid_generator;
using namespace grid_compressor;

int distrib_test( )
{
	NormalDistribution d( 2, 3 );
	
	Quantizer quantizer( 2, 10 );
	Quantization q = quantizer.quantize( 10, 0.01, d );

	cout << "borders:" << endl;

	for each ( double border in q.borders )
		cout << border << ' ';
	cout << endl << endl;

	cout << "codes:" << endl;

	for each ( double code in q.codes )
		cout << code << ' ';
	cout << endl << endl;

	cout << "deviation:" << endl;
	cout << q.deviation << endl;

	/*vector<pIDistribution> distrs = { make_heap_aware<NormalDistribution>( 0, 1 ) };

	auto result = generate_grid( 10, distrs );

	for ( int i = 0; i < result->get_row_count( ); ++i )
	{
		for ( int j = 0; j < result->get_column_count( ); ++j )
			cout << result->get_value( i, j ) << ' ';
		cout << endl;
	}*/

	system( "pause" );

	return 0;
}

void write_grid( string file_name, const pIGrid &grid )
{
	if ( grid->get_row_count( ) == 0 ||
		 grid->get_column_count( ) == 0 )
		return;

	ofstream out = ofstream( file_name.c_str( ) );

	for ( int i = 0; i < grid->get_row_count( ); ++i )
	{
		out << grid->get_value( i, 0 );
		for ( int j = 1; j < grid->get_column_count( ); ++j )
			out << ";" << grid->get_value( i, j );
		out << endl;
	}
}

#define QUANT_COUNT 100
#define ROW_COUNT 1e6
#define EPS 1e-1

int main_io( )
{
	{
		FileOutputStream stream( L"test.txt" );
		stream << 2;
	}
	{
		FileInputStream stream( L"test.txt" );
		int a;
		stream >> a;
		cout << a;
	}

	system( "pause" );

	return 0;
}

using time::Timer;

#include <IIntegrator.h>
#include <TrapezoidalIntegrator.h>

using namespace integrators;

int main_integrate( )
{
	auto integrator = TrapezoidalIntegrator( 1e-4 );

	double result = integrator.integrate( []( double x ) { return x; }, 0.0, 6.0 );
	cout << result << endl;

	system( "pause" );

	return 0;
}

#include <EmpiricalDistribution.h>
#include <cmath>

int main( )
{
	vector<pIDistribution> distrs = { make_heap_aware<NormalDistribution>( 0, 1 ) };

	auto grid = generate_grid( ROW_COUNT, distrs );

	double min = numeric_limits<double>::max( ),
		max = numeric_limits<double>::min( );

	for ( int i = 0; i < ROW_COUNT; ++i )
	{
		min = min < grid->get_value( i, 0 ) ? min : grid->get_value( i, 0 );
		max = max > grid->get_value( i, 0 ) ? max : grid->get_value( i, 0 );
	}

	double k = 1.0 / sqrt( 2 * _Pi );

	ApproximationMethod method = { [k]( double r ) { return k * exp( -0.5 * r * r ); }, 0.35 };

	auto empirical = EmpiricalDistribution( grid, 0, method, min, max, 1e-2 );

	cout << "Alpha = " << empirical.expectation( ) << endl;
	cout << "Sigma = " << empirical.deviation( ) << endl;

	system( "pause" );
}

int main_archive( )
{
	vector<pIDistribution> distrs = { make_heap_aware<NormalDistribution>( 0, 1 ), make_heap_aware<NormalDistribution>( 2, 4 ) };

	auto grid = generate_grid( ROW_COUNT, distrs );

	Timer timer;
	double quatize_time;
	Quantizations qs;

	timer.start( );

	for ( int column = 0; column < distrs.size( ); ++column )
	{
		double left = numeric_limits<double>::max( );
		double right = numeric_limits<double>::min( );

		for ( int row = 0; row < grid->get_row_count( ); ++row )
		{
			auto value = grid->get_value( row, column );
			left = left < value ? left : value;
			right = right > value ? right : value;
		}

		left -= EPS;
		right += EPS;

		Quantizer quantizer( left, right );
		qs.push_back( quantizer.quantize( QUANT_COUNT, EPS, *distrs[column] ) );
	}

	quatize_time = timer.stop( );
	cout << "Quantization time: " << quatize_time << "s" << endl;

	huffman::HuffmanCompressor compressor;

	vector<double> bps_counts;

	double compression_time, decompression_time;

	{
		FileOutputStream stream( L"output.txt" );
		timer.start( );
		bps_counts = compressor.compress( grid, qs, stream );
		compression_time = timer.stop( );
	}

	cout << "Compression time: " << compression_time << "s" << endl;

	write_grid( "grid.csv", grid );
	
	{
		FileInputStream stream( L"output.txt" );

		timer.start( );
		auto decompressed = compressor.decompress( stream );
		decompression_time = timer.stop( );

		cout << "Decompression time: " << decompression_time << "s" << endl;
		cout << endl;

		write_grid( "decompressed.csv", decompressed );

#pragma region ERROR_COUNT

		vector<double> max_error( grid->get_column_count( ) ),
			min_error( grid->get_column_count( ) ),
			avg_error( grid->get_column_count( ) ),
			ms_error( grid->get_column_count( ) ),
			left_borders( grid->get_column_count( ) ),
			right_borders( grid->get_column_count( ) );

		for ( auto &val : min_error )
			val = numeric_limits<double>::max( );

		for ( auto &val : left_borders )
			val = numeric_limits<double>::max( );

		for ( auto &val : right_borders )
			val = numeric_limits<double>::min( );

		for ( int i = 0; i < grid->get_row_count( ); ++i )
		{
			for ( int j = 0; j < grid->get_column_count( ); ++j )
			{
				left_borders[j] = grid->get_value( i, j ) > left_borders[j] ? left_borders[j] : grid->get_value( i, j );
				right_borders[j] = grid->get_value( i, j ) < right_borders[j] ? right_borders[j] : grid->get_value( i, j );

				auto error = fabs( grid->get_value( i, j ) -
								   decompressed->get_value( i, j ) );

				auto square_error = pow( ( grid->get_value( i, j ) -
										   decompressed->get_value( i, j ) ), 2 );

				max_error[j] = max_error[j] > error ? max_error[j] : error;
				min_error[j] = min_error[j] < error ? min_error[j] : error;
				
				if ( i > 0 )
				{
					avg_error[j] = ( avg_error[j] * i + error ) / ( i + 1 );
				}
				else
				{
					avg_error[j] = error;
				}

				if ( i > 0 )
				{
					ms_error[j] = ( ms_error[j] * i + square_error ) / ( i + 1 );
				}
				else
				{
					ms_error[j] = square_error;
				}
			}
		}

		for ( int i = 0; i < grid->get_column_count( ); ++i )
		{
			cout << "Info for column " << i << ":" << endl << endl;

			cout << "Interval: " << "[" << left_borders[i] << "; " << right_borders[i] << "]" << endl;
			cout << "Expected entropy = " << qs[i].entropy << endl;
			cout << "Average bit count per symbol = " << bps_counts[i] << endl;
			cout << "Expected mean square deviation = " << qs[i].deviation << endl;
			cout << "Real mean square deviation = " << ms_error[i] << endl;
			cout << "Minimal deviation = " << min_error[i] << endl;
			cout << "Maximal deviation = " << max_error[i] << endl;
			cout << "Average deviation = " << avg_error[i] << endl;
			cout << endl << endl;
		}

		system( "pause" );
#pragma endregion ERROR_COUNT

	}


	return 0;
}