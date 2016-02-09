#pragma once
#include "Quantpressor.h"

#include <io.h>

namespace Quantpressor
{
	ref class NativeGrid : public IGrid
	{
	private:
		module_api::pIGrid *nativeGrid;

	internal:
		module_api::pIGrid NativePtr( );

	public:
		NativeGrid( module_api::pIGrid &&grid );

		!NativeGrid( );

		virtual double GetValue( int row, int column );
		
		virtual property int RowCount
		{
			int get( );
		}
		virtual property int ColumnCount
		{
			int get( );
		}
	};
}