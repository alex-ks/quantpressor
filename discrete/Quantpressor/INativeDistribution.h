#pragma once
#include "Quantpressor.h"

#include <IDistribution.h>

namespace Quantpressor
{
	interface class INativeDistribution : public IDistribution
	{
	public:
		distributions::IDistribution *NativePtr( );
	};
}
