// Quantpressor.h

#pragma once

using namespace System;

namespace Quantpressor
{
	public interface class IDistribution
	{
	public:
		double Density( double x );
		double Cumulative( double x );

		property double TotalExpectation
		{
			double get( );
		}

		double Expectation( double a, double b );

		property double TotalVariance
		{
			double get( );
		}

		double Variance( double a, double b );
	};

	public interface class IBinaryInputStream : public IDisposable
	{
	public:
		int ReadInt( );
		System::Int64 ReadLong( );
		System::Byte ReadByte( );
		double ReadDouble( );

		bool ReadBit( );
		int ReadBytes( array<System::Byte> ^bytes, int count );

		property bool EndOfStream
		{
			bool get( );
		}
	};

	public interface class IBinaryOutputStream : public IDisposable
	{
	public:
		void Write( int );
		void Write( System::Int64 );
		void Write( System::Byte );
		void Write( double );

		void WriteBit( bool bit );
		void WriteBytes( array<System::Byte> ^bytes, int count );

		void Flush( );
	};

	public interface class IQuantization
	{
	public:
		property array<double> ^Borders
		{
			array<double> ^get( );
		}

		property array<double> ^Codes
		{
			array<double> ^get( );
		}

		property Double Variance
		{
			double get( );
		}

		property Double Entropy
		{
			double get( );
		}
	};

	public interface class IGrid
	{
	public:
		double GetValue( int row, int column );

		property int RowCount
		{
			int get( );
		}

		property int ColumnCount
		{
			int get( );
		}
	};

	public interface class ICompressionResult
	{
	public:
		property array<double> ^ColumnsBitsPerSymbol
		{
			array<double> ^get( );
		}

		property array<double> ^RealVariances
		{
			array<double> ^get( );
		}

		property array<double> ^MinErrors
		{
			array<double> ^get( );
		}

		property array<double> ^AvgErrors
		{
			array<double> ^get( );
		}

		property array<double> ^MaxErrors
		{
			array<double> ^get( );
		}
	};

	public interface class ICompressor
	{
	public:
		ICompressionResult ^Compress( IGrid ^grid,
									  System::Collections::Generic::IList<IQuantization ^> ^quantizations,
									  IBinaryOutputStream ^stream );

		IGrid ^Decompress( IBinaryInputStream ^stream );
	};

	public interface class IGridReader : public IDisposable
	{
	public:
		IGrid ^Read( System::String ^fileName,
					 bool hasRowNames,
					 bool hasColumnNames );
	};

	public interface class IGridWriter : public IDisposable
	{
	public:
		void Write( System::String ^fileName,
					IGrid ^grid );
	};
}

