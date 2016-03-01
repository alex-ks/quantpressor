#pragma once
#include "TwoDimensionalArray.h"

#include <io.h>

namespace quantpressor
{
	namespace column_sort
	{
		class CorrelationCounter
		{
		public:
			double operator( ) ( const module_api::pIGrid &grid, 
								 module_api::uint first_column, 
								 module_api::uint second_column ) const;

			TwoDimensionalArray<double> operator( ) ( const module_api::pIGrid &grid ) const;
		};
	}
}
