#pragma once

#include "io.h"
#include "method.h"

namespace igor
{
	class RoughMethod : public module_api::IMethod
	{
	private:
		//There should be session-unique data, not in global scope
		module_api::pISystem god_object;

	public:
		RoughMethod( const module_api::pISystem &god_object );
		~RoughMethod( );

		virtual module_api::pIResult run( const module_api::pIArguments &arguments, const module_api::pINotifier &notifier ) override;
	};
}