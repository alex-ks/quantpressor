#pragma once
#include "io.h"

namespace igor
{
	using module_api::uint;

	class RoughGrid : public module_api::IGrid
	{
	private:
		double **grid;
		uint row_count, column_count;
		module_api::pIMetadata metadata;
		RoughGrid( const RoughGrid& ) = delete;

	public:
		RoughGrid( int row_count, int column_count );
		RoughGrid( const IGrid& );

		double get_value( uint row, uint column ) const override;

		module_api::DoubleArray get_rect( uint start_row, uint start_column, uint row_count, uint column_count ) const override;

		uint get_column_count( ) const override;
		uint get_row_count( ) const override;

		module_api::pIMetadata get_metadata( ) const override;

		virtual void set_value( uint row, uint column, double value );
		void set_metadata( const module_api::pIMetadata &new_metadata );

		virtual ~RoughGrid( );
	};
}