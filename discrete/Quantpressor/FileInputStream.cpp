#include "stdafx.h"
#include "FileInputStream.h"

#include <vcclr.h>

quantpressor::io::FileInputStream * Quantpressor::FileIO::FileInputStream::NativePtr( )
{
	if ( nativeStream == nullptr )
		throw gcnew System::ObjectDisposedException( this->ToString( ) );
	return nativeStream;
}

Quantpressor::FileIO::FileInputStream::FileInputStream( System::String ^fileName )
{
	pin_ptr<const wchar_t> wpath = PtrToStringChars( fileName );
	nativeStream = new quantpressor::io::FileInputStream( wpath );
}

Quantpressor::FileIO::FileInputStream::~FileInputStream( )
{
	delete nativeStream;
	nativeStream = nullptr;
}

Quantpressor::FileIO::FileInputStream::!FileInputStream( )
{
	if ( nativeStream == nullptr )
	{
		delete nativeStream;
		nativeStream = nullptr;
	}
}

#pragma unmanaged

template<typename T> T ReadSmth( quantpressor::io::FileInputStream &stream )
{
	T result;
	stream >> result;
	return result;
}

#pragma managed

int Quantpressor::FileIO::FileInputStream::ReadInt( )
{
	return ReadSmth<int>( *nativeStream );
}

System::Int64 Quantpressor::FileIO::FileInputStream::ReadLong( )
{
	return ReadSmth<unsigned long long>( *nativeStream );
}

System::Byte Quantpressor::FileIO::FileInputStream::ReadByte( )
{
	return ReadSmth<unsigned char>( *nativeStream );
}

double Quantpressor::FileIO::FileInputStream::ReadDouble( )
{
	return ReadSmth<double>( *nativeStream );
}

bool Quantpressor::FileIO::FileInputStream::ReadBit( )
{
	return nativeStream->read_bit( );
}

int Quantpressor::FileIO::FileInputStream::ReadBytes( array<System::Byte>^ bytes, int count )
{
	if ( bytes->Length < count )
		throw gcnew System::IndexOutOfRangeException( );

	pin_ptr<System::Byte> ptr = &bytes[0];
	return nativeStream->read_bytes( ptr, count );
}
