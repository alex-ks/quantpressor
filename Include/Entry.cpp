#define DLL_EXPORT

#include "io.h"

#include "RoughMethod.h"

using module_api::pIMethod;
using module_api::pISystem;
using module_api::make_heap_aware;

using igor::RoughMethod;

DLL_API pIMethod module_api::create_instance( const pISystem &sys )
{
	return make_heap_aware<RoughMethod>( sys );
}