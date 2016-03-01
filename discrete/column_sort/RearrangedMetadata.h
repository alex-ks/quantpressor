#pragma once

#include <io.h>
#include <vector>

namespace quantpressor
{
	namespace column_sort
	{
		class RearrangedMetadata : public module_api::IMetadata
		{
		private:
			std::vector<module_api::uint> order;
			std::vector<std::wstring> column_names;
			module_api::pIMetadata metadata;

		public:
			RearrangedMetadata( const module_api::pIMetadata &origin, std::vector<module_api::uint> new_column_order );
			~RearrangedMetadata( ) override;

			const std::vector<std::wstring>& get_replacement( module_api::uint index ) const override;

			module_api::ReplacementState get_replacements_state( ) const override;

			std::wstring get_column_name( module_api::uint column ) const override;

			const std::vector<std::wstring>& get_column_names( ) const override;

			module_api::uint get_column_count( ) const override;

			void set_column_name( module_api::uint column, const std::wstring & new_name ) override;

			void set_column_names( const std::vector<std::wstring>& names ) override;

			std::wstring get_row_name( module_api::uint row ) const override;

			const std::vector<std::wstring>& get_row_names( ) const override;

			module_api::uint get_row_count( ) const override;

			void set_row_name( module_api::uint row, const std::wstring &new_name ) override;

			void set_row_names( const std::vector<std::wstring>& names ) override;
		};
	}
}