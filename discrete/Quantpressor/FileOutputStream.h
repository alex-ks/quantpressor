#pragma once
#include "Quantpressor.h"

#include <FileOutputStream.h>

namespace Quantpressor
{
	namespace FileIO
	{
		public ref class FileOutputStream : public IBinaryOutputStream
		{
		private:
			quantpressor::io::FileOutputStream *nativeStream;

		internal:
			quantpressor::io::FileOutputStream *NativePtr( );

		public:
			FileOutputStream( System::String ^fileName );
			~FileOutputStream( );
			!FileOutputStream( );

			virtual void Write( int );

			virtual void Write( System::Int64 );

			virtual void Write( System::Byte );

			virtual void Write( double );

			virtual void WriteBit( bool bit );

			virtual void WriteBytes( array<System::Byte>^ bytes, int count );

			virtual void Flush( );
		};
	}
}