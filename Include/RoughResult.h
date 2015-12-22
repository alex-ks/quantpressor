#pragma once
#include "io.h"
#include <vector>

namespace igor
{
	class RoughResult : public module_api::IResult
	{
	private:
		std::vector<module_api::pIGrid> grids;
		module_api::Paragraphs plain_text;

		RoughResult( const RoughResult& ) = delete;

	public:
		RoughResult( );
		RoughResult( module_api::Paragraphs &&text );
		RoughResult( const module_api::Paragraphs &text );

		module_api::pIGrid get_grid( module_api::uint i ) const override;
		module_api::uint get_grids_count( ) const override;

		module_api::Paragraphs get_plain_text( ) const override;

		void add_grid( const module_api::pIGrid &grid );

		void set_plain_text( module_api::Paragraphs &&text );
		void set_plain_text( const module_api::Paragraphs &text );

		virtual ~RoughResult( );
	};
}