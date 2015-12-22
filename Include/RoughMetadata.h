#pragma once

#include <vector>
#include "io.h"

namespace igor
{
	class RoughMetadata : public module_api::IMetadata
	{
	private:
		std::vector<std::vector<std::wstring>> replacements;
		module_api::uint column_count, row_count;
		std::vector<std::wstring> column_names, row_names;
		module_api::ReplacementState replacements_state;
		RoughMetadata( const RoughMetadata& ) = delete;

	public:
		RoughMetadata( module_api::uint row_count, module_api::uint column_count );
		RoughMetadata( const IMetadata& );

		virtual std::wstring get_column_name( module_api::uint column ) const override;
		virtual const std::vector<std::wstring> &get_column_names( ) const override;
		virtual module_api::uint get_column_count( ) const override;

		virtual std::wstring get_row_name( module_api::uint row ) const override;
		virtual const std::vector<std::wstring> &get_row_names( ) const override;
		virtual module_api::uint get_row_count( ) const override;

		virtual const std::vector<std::wstring> &get_replacement( module_api::uint index ) const override;
		virtual module_api::ReplacementState get_replacements_state( ) const override;

		virtual void set_row_name( module_api::uint row_num, const std::wstring &new_name ) override;
		virtual void set_row_names( const std::vector<std::wstring> &names ) override;
		virtual void set_column_name( module_api::uint column_num, const std::wstring &new_name ) override;
		virtual void set_column_names( const std::vector<std::wstring> &names ) override;

		void set_replacement( module_api::uint column_num, const std::vector<std::wstring> &new_replacement );
		void set_replacemnts_state( module_api::ReplacementState new_state );

		~RoughMetadata( ) override;
	};
}