#pragma once
#include "Quantizer.h"
#include <vector>
#include <list>
#include <functional>

namespace quantpressor
{
	class GridQuantizer
	{
	public:
		std::vector<Quantization> quantize_grid( const module_api::pIGrid & grid,
												 const std::vector<double>& max_errors ) const;
	};
}