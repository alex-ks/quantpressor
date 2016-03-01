#include <vector>
#include <iostream>
#include <fstream>

#include <lib.h>
#include <Quantizer.h>
#include <HuffmanCompressor.h>
#include <LZ77HuffmanCompressor.h>
#include <NormalDistribution.h>
#include <ExponentialDistribution.h>

#include <ColumnSorter.h>

#include <FileInputStream.h>
#include <FileOutputStream.h>

#include "Timer.h"

using namespace std;
using namespace module_api;
using namespace distributions;
using namespace grid_generator;
using namespace quantpressor;
using namespace quantpressor::io;

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

#define QUANT_COUNT 30
#define ROW_COUNT 1e6
#define EPS 1e-2

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

#include <FastEmpiricalDistribution.h>

int main_distr_replicate( )
{
	vector<pIDistribution> distrs = { make_heap_aware<NormalDistribution>( 2, 3 ) };

	auto grid = generate_grid( ROW_COUNT, distrs );

	double k = 1.0 / sqrt( 2 * _Pi );
	const double _sq2 = 1.0 / sqrt( 2.0 );

	auto gauss_kernel = [k]( double r ) { return k * exp( -0.5 * r * r ); };

	auto method = DetailedApproximationMethod::gauss_kernel( 0.35 );

	auto empirical = FastEmpiricalDistribution( grid, 0, method );

	cout << "Alpha = " << empirical.expectation( ) << endl;
	cout << "Sigma = " << sqrt( empirical.deviation( ) ) << endl;

	system( "pause" );

	return 0;
}

#include <CsvReader.h>

int main_read( )
{
	int buf_size = 0;
	
	cin >> buf_size;

	while ( buf_size != -1 )
	{
		CsvReader reader( buf_size, L';' );

		try
		{
			auto grid = reader.read( L"grid.csv", false, false );
			cout << grid->get_row_count( ) << " " << grid->get_column_count( ) << endl;
		}
		catch ( unsigned int e )
		{
			cout << e << endl;
		}

		cin >> buf_size;
	}

	return 0;
}

int main_archive_normal( )
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

	compressors::HuffmanCompressor compressor;

	CompressionResult compress_params;

	double compression_time, decompression_time;

	{
		FileOutputStream stream( L"output.txt" );
		timer.start( );
		compress_params = compressor.compress( grid, qs, stream );
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
			cout << "Average bit count per symbol = " << compress_params.columns_bps[i] << endl;
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

int main/*_archive_empirical*/( )
{
	auto reader = CsvReader( 1024 * 1024, L';' );
	auto grid = reader.read( L"real_data.csv", false, false );
	wstring archive_name = L"real.qlz";
	//wstring archive_name = L"real.out";

	cout << "Grid loaded" << endl;

	vector<pIDistribution> distrs;

	Timer timer;
	double quatize_time;
	Quantizations qs;

	auto method = DetailedApproximationMethod::gauss_kernel( 0.35 );

	timer.start( );

	vector<double> 
		left_borders( grid->get_column_count( ) ),
		right_borders( grid->get_column_count( ) );

	for ( int column = 0; column < grid->get_column_count( ); ++column )
	{
		double left = numeric_limits<double>::max( );
		double right = numeric_limits<double>::min( );

		for ( int row = 0; row < grid->get_row_count( ); ++row )
		{
			auto value = grid->get_value( row, column );
			left = left < value ? left : value;
			right = right > value ? right : value;
		}

		left_borders[column] = left;
		right_borders[column] = right;

		left -= EPS;
		right += EPS;

		pIDistribution empirical = make_heap_aware<FastEmpiricalDistribution>( grid, column, method );

		distrs.push_back( empirical );

		Quantizer quantizer( left, right );
		qs.push_back( quantizer.quantize( QUANT_COUNT, EPS, *empirical ) );

		cout << "Column " << column << " quantized" << endl;
	}

	quatize_time = timer.stop( );
	cout << "Quantization time: " << quatize_time << "s" << endl;

	//compressors::HuffmanCompressor compressor;
	compressors::LZ77HuffmanCompressor compressor( 1024 * 10 );

	CompressionResult compression_params;

	double compression_time, decompression_time;

	{
		FileOutputStream stream( archive_name );
		timer.start( );
		compression_params = compressor.compress( grid, qs, stream );
		compression_time = timer.stop( );
	}

	cout << "Compression time: " << compression_time << "s" << endl;

	pIGrid decompressed;

	{
		FileInputStream stream( archive_name );
		timer.start( );
		decompressed = compressor.decompress( stream );
		decompression_time = timer.stop( );
	}

	cout << "Decompression time: " << decompression_time << "s" << endl;
	cout << endl;

	write_grid( "decompressed.csv", decompressed );
	cout << "Decompressed written" << endl;

	for ( int i = 0; i < grid->get_column_count( ); ++i )
	{
		cout << "Info for column " << i << ":" << endl << endl;

		cout << "Interval: " << "[" << left_borders[i] << "; " << right_borders[i] << "]" << endl;
		cout << "Expected entropy = " << qs[i].entropy << endl;
		cout << "Average bit count per symbol = " << compression_params.columns_bps[i] << endl;
		cout << "Expected mean square deviation = " << qs[i].deviation << endl;
		cout << "Real mean square deviation = " << compression_params.real_variances[i] << endl;
		cout << "Minimal deviation = " << compression_params.min_errors[i] << endl;
		cout << "Maximal deviation = " << compression_params.max_errors[i] << endl;
		cout << "Average deviation = " << compression_params.avg_errors[i] << endl;
		cout << endl << endl;
	}

	for ( auto &pair : compression_params.extra_results )
	{
		wcout << pair.first << L" = " << pair.second << endl;
	}

	system( "pause" );

	return 0;
}

#include <huffman.h>

using namespace quantpressor::compressors::huffman;

int main_dynamic_huffman( )
{
	map<char, ull> freq;

	freq['a'] = 1;
	freq['b'] = 2;

	DynamicHuffmanTree<char> tree( freq, -1 );

	std::wstring file_name = L"tree.out";

	{
		auto stream = FileOutputStream( file_name );
		tree.serialize( stream );
	}

	{
		auto stream = FileInputStream( file_name );
		auto tree_copy = DynamicHuffmanTree<char>::deserialize( stream, -1 );
		tree_copy.assume_frequences( freq );
	}

	bool res = tree.add_new_symbol( 'a' );
	res = tree.add_new_symbol( 'a' );
	res = tree.add_new_symbol( 'c' );

	return 0;
}

int main_fast_empirical( )
{
	pIGrid grid;

	{
		auto reader = CsvReader( 1024 * 1024, L';' );
		grid = reader.read( L"real_data.csv", false, false );
	}

	auto method = DetailedApproximationMethod::gauss_kernel( 0.35 );

	auto grid_driven = FastEmpiricalDistribution( grid, 0, method );

	auto count = grid->get_row_count( );
	int step = 1;
	auto maxSampleCount = 1000U;

	if ( count > maxSampleCount )
	{
		step = count / maxSampleCount;
		count = maxSampleCount;
	}

	std::vector<double> sample( count );

	for ( int i = 0; i < count; ++i )
	{
		sample[i] = grid->get_value( i * step, 0 );
	}

	auto sample_driven = FastEmpiricalDistribution( std::move( sample ), method );

	return 0;
}

using compressors::LZ77HuffmanCompressor;
using compressors::HuffmanCompressor;

class EmptyOutputStream : public IBinaryOutputStream
{
private: 
	ull pos = 0;

public:
	ull get_current_position( ) const override { return pos; }
	IBinaryOutputStream & operator<<( int ) override { return *this; }
	IBinaryOutputStream & operator<<( unsigned int ) override { return *this; }
	IBinaryOutputStream & operator<<( unsigned long long ) override { return *this; }
	IBinaryOutputStream & operator<<( char ) override { return *this; }
	IBinaryOutputStream & operator<<( byte ) override { return *this; }
	IBinaryOutputStream & operator<<( double ) override { return *this; }
	void write_bit( bool bit ) override { ++pos; }
	void write_bytes( byte * bytes, unsigned int count ) override { }
	void flush( ) override { }
};

int main_lz77( )
{
	auto reader = CsvReader( 1024, L';' );
	auto grid = reader.read( L"lztest.csv", false, false );

	Quantizations qs;
	Quantization q = Quantization( 3 );

#pragma region HARDCODE
	q.borders = { 0.5, 1.5, 2.5 };
	q.codes = { 1, 2 };
	q.deviation = 0;
	q.entropy = 1;
	qs.push_back( q );
	q.borders = { 1, 3, 5 };
	q.codes = { 2, 4 };
	qs.push_back( q );
	q.borders = { -1, 1, 3 };
	q.codes = { 0, 2 };
	qs.push_back( q );
	q.borders = { 0, 2, 4 };
	q.codes = { 1, 3 };
	qs.push_back( q );
#pragma endregion

	auto lz_compressor = LZ77HuffmanCompressor( 1024 );
	auto h_compressor = HuffmanCompressor( );
	auto lz_stream = EmptyOutputStream( );
	auto h_stream = EmptyOutputStream( );

	auto result = lz_compressor.compress( grid, qs, lz_stream );
	result = h_compressor.compress( grid, qs, h_stream );

	{
		auto stream = FileOutputStream( L"output.out" );
		lz_compressor.compress( grid, qs, stream );
	}

	pIGrid result_grid;

	{
		auto stream = FileInputStream( L"output.out" );
		result_grid = lz_compressor.decompress( stream );
	}

	return 0;
}
