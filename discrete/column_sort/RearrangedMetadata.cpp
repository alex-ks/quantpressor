#include "RearrangedMetadata.h"

namespace quantpressor
{
	namespace column_sort
	{
		using namespace module_api;
		using std::vector;

		RearrangedMetadata::RearrangedMetadata( const pIMetadata &origin, vector<uint> new_column_order ) : 
			metadata( origin ),
			order( std::move( new_column_order ) ),
			column_names( origin->get_column_count( ) )
		{
			for ( int i = 0; i < column_names.size( ); ++i )
			{
				column_names[i] = metadata->get_column_name( order[i] );
			}
		}

		RearrangedMetadata::~RearrangedMetadata( )
		{
		
		}

		const vector<std::wstring>& RearrangedMetadata::get_replacement( uint index ) const
		{
			if ( metadata->get_replacements_state( ) == ReplacementState::REPLACES_COLUMNS )
			{
				return metadata->get_replacement( order[index] );
			}
			else
			{
				return metadata->get_replacement( index );
			}
		}

		ReplacementState RearrangedMetadata::get_replacements_state( ) const
		{
			return metadata->get_replacements_state( );
		}

		std::wstring RearrangedMetadata::get_column_name( uint column ) const
		{
			return column_names[column];
		}

		const std::vector<std::wstring>& RearrangedMetadata::get_column_names( ) const
		{
			return column_names;
		}

		uint RearrangedMetadata::get_column_count( ) const
		{
			return metadata->get_column_count( );
		}

		void RearrangedMetadata::set_column_name( uint column, const std::wstring &new_name )
		{
			metadata->set_column_name( order[column], new_name );
			column_names[column] = new_name;
		}

		void RearrangedMetadata::set_column_names( const std::vector<std::wstring>& names )
		{
			auto temp_names = names;
			for ( int i = 0; i < names.size( ) && i < order.size( ); ++i )
			{
				temp_names[i] = names[order[i]];
			}
			metadata->set_column_names( names );
			column_names = std::move( temp_names );
		}

		std::wstring RearrangedMetadata::get_row_name( uint row ) const
		{
			return metadata->get_row_name( row );
		}

		const std::vector<std::wstring>& RearrangedMetadata::get_row_names( ) const
		{
			return metadata->get_row_names( );
		}

		uint RearrangedMetadata::get_row_count( ) const
		{
			return metadata->get_row_count( );
		}

		void RearrangedMetadata::set_row_name( uint row, const std::wstring & new_name )
		{
			metadata->set_row_name( row, new_name );
		}

		void RearrangedMetadata::set_row_names( const std::vector<std::wstring>& names )
		{
			metadata->set_row_names( names );
		}
	}
}