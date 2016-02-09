#include "stdafx.h"
#include "NativeQuantization.h"

using namespace Quantpressor;

grid_compressor::Quantization NativeQuantization::NativeStruct( )
{
	return *nativeQuantization;
}

Quantpressor::NativeQuantization::NativeQuantization( grid_compressor::Quantization && quantization )
{
	nativeQuantization = new grid_compressor::Quantization( 1 );
	*nativeQuantization = std::move( quantization );
}

Quantpressor::NativeQuantization::!NativeQuantization( )
{
	delete nativeQuantization;
	nativeQuantization = nullptr;
}

array<double> ^NativeQuantization::Borders::get( )
{
	auto result = gcnew array<double>( nativeQuantization->borders.size( ) );

	for ( int i = 0; i < nativeQuantization->borders.size( ); ++i )
	{
		result[i] = nativeQuantization->borders[i];
	}

	return result;
}

array<double> ^NativeQuantization::Codes::get( )
{
	auto result = gcnew array<double>( nativeQuantization->codes.size( ) );

	for ( int i = 0; i < nativeQuantization->codes.size( ); ++i )
	{
		result[i] = nativeQuantization->codes[i];
	}

	return result;
}

double NativeQuantization::Entropy::get( )
{
	return nativeQuantization->entropy;
}

double NativeQuantization::Variance::get( )
{
	return nativeQuantization->deviation;
}