#include "DelayedReceiver.h"

namespace module_api
{
	using std::unique_lock;

	DelayedReceiver::DelayedReceiver( ) : 
		result( nullptr ), 
		exception_ptr( nullptr ),
		ready( false )
	{
	
	}

	DelayedReceiver::~DelayedReceiver( )
	{
	
	}
	
	void DelayedReceiver::receive_result( const pIResult & result )
	{
		unique_lock<std::mutex> lock( mutex );
		this->result = result;
		ready = true;
		var_to_wait_on.notify_one( );
	}
	
	void DelayedReceiver::catch_exception( const Exception & exception )
	{
		unique_lock<std::mutex> lock( mutex );
		this->exception_ptr = make_heap_aware<Exception>( exception );
		ready = true;
		var_to_wait_on.notify_one( );
	}
	
	pIResult DelayedReceiver::get_result( )
	{
		unique_lock<std::mutex> lock( mutex );
		while ( !ready )
			var_to_wait_on.wait( lock );

		if ( exception_ptr != nullptr )
		{
			throw *exception_ptr;
		}

		return result;
	}
	
	bool DelayedReceiver::is_ready( ) const noexcept
	{
		return ready;
	}
}