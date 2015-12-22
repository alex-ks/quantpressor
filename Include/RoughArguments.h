#pragma once
#include "io.h"
#include <vector>

namespace igor
{
	class RoughArguments : public module_api::IArguments
	{
	private:
		std::vector<module_api::pIGrid> grids;
		bool analytics_flag;

	public:
		RoughArguments( );
		RoughArguments( const module_api::IArguments &origin );
		~RoughArguments( );

		virtual module_api::pIGrid get_grid( module_api::uint number ) const override;
		virtual module_api::uint get_grid_count( ) const override;

		virtual void add_grid( const module_api::pIGrid &grid );

		virtual bool make_analytics( ) const override;
	};
}
