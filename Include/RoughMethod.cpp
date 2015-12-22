#include "IO.h"
#include "RoughMethod.h"
#include "RoughGrid.h"
#include "RoughResult.h"

#include "exception.h"

#include "memory.h"

#include <Windows.h>

using module_api::pIResult;
using module_api::pIArguments;
using module_api::pINotifier;
using module_api::Notification;
using module_api::HeapPtr;
using module_api::make_heap_aware;
using module_api::Paragraphs;

using module_api::operator+;

igor::RoughMethod::RoughMethod( const module_api::pISystem &god_object ) : god_object( god_object )
{

}

igor::RoughMethod::~RoughMethod( )
{

}

pIResult igor::RoughMethod::run( const pIArguments &arguments, const pINotifier &notifier )
{
	notifier->set_progress( 0.0 );
	auto resources = god_object->get_module_resources( );

	if ( arguments->get_grid_count( ) == 0 )
		throw module_api::BadInputException( resources[L"bad_input_msg"] );

	//shared_ptr<RoughGrid> result_grid = make_shared<RoughGrid>( *arguments->get_grid( 0 ) );
	notifier->notify( Notification( L"igor_stub", resources[L"grid_alloc_msg"] ) );

	HeapPtr<RoughGrid> result_grid =  HeapPtr<RoughGrid>( new RoughGrid( *arguments->get_grid( 0 ) ) );

	notifier->set_progress( 10.0 );

	int element_count = result_grid->get_column_count( ) * result_grid->get_row_count( );

	unsigned long execution_time = 2000,
		sleep_time = execution_time / element_count;

	double progress_add = 90.0 / element_count;
	double current_progress = 10.0;

	notifier->notify( Notification( L"igor_stub", resources[L"value_calc_msg"] ) );

	for ( uint i = 0; i < result_grid->get_row_count( ); ++i )
		for ( uint j = 0; j < result_grid->get_column_count( ); ++j )
		{
			Sleep( sleep_time );
			result_grid->set_value( i, j, result_grid->get_value( i, j ) + 1 );
			current_progress += progress_add;
			notifier->set_progress( current_progress );
		}

	notifier->notify( Notification( L"igor_stub", resources[L"interpret_msg"] ) );

	Paragraphs text;

	text.push_back( resources[L"hello_string"] );
	text.push_back( resources[L"interpret_abstract"] );
	text.push_back( resources[L"row_count_str"] + result_grid->get_row_count( ) );
	text.push_back( resources[L"column_count_str"] + result_grid->get_column_count( ) );

	HeapPtr<RoughResult> result = make_heap_aware<RoughResult>( std::move( text ) );
	result->add_grid( result_grid );

	notifier->notify( Notification( L"igor_stub", resources[L"ready_msg"] ) );

	notifier->notify_of_session_finished( );
	return result;
}
