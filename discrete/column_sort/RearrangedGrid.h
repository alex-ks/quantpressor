#pragma once
#include <io.h>
#include <vector>

namespace quantpressor
{
	namespace column_sort
	{
		class RearrangedGrid : public module_api::IGrid
		{
		private:
			module_api::pIGrid grid;
			module_api::pIMetadata metadata;
			std::vector<module_api::uint> order;

		public:
			RearrangedGrid( const module_api::pIGrid &origin, std::vector<module_api::uint> &&new_column_order );
			~RearrangedGrid( ) override;

			double get_value( module_api::uint row, module_api::uint column ) const override;
			module_api::DoubleArray get_rect( module_api::uint min_row, module_api::uint min_column, module_api::uint row_count, module_api::uint column_count ) const override;
			module_api::uint get_column_count( ) const override;
			module_api::uint get_row_count( ) const override;
			module_api::pIMetadata get_metadata( ) const override;
		};
	}
}