#pragma once
#include "Quantpressor.h"

#include <FileInputStream.h>

namespace Quantpressor
{
	namespace FileIO
	{
		public ref class FileInputStream : public IBinaryInputStream
		{
		private:
			grid_compressor::FileInputStream *nativeStream;

		internal:
			grid_compressor::FileInputStream *NativePtr( );

		public:
			FileInputStream( System::String ^fileName );
			~FileInputStream( );
			!FileInputStream( );

			// Inherited via IBinaryInputStream
			virtual int ReadInt( );

			virtual System::Int64 ReadLong( );

			virtual System::Byte ReadByte( );

			virtual double ReadDouble( );

			virtual bool ReadBit( );

			virtual int ReadBytes( array<System::Byte>^ bytes, int count );

			virtual property bool EndOfStream;
		};
	}
}
