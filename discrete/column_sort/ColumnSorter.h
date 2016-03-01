#pragma once

#include <io.h>
#include <vector>

namespace quantpressor
{
	namespace column_sort
	{
		class ColumnSorter
		{
		public:
			module_api::pIGrid sort_columns( const module_api::pIGrid &grid );
		};
	}
}

