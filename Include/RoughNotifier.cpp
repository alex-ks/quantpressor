#include "RoughNotifier.h"

using namespace igor;

using module_api::Notification;
using module_api::pISubscriber;

void RoughNotifier::notify( Notification notification )
{
	for ( auto subscriber : subscribers )
		subscriber->receive_notification( notification );
}

void RoughNotifier::set_progress( double progress )
{
	progress = progress < 100.0 ? progress : 100.0;
	progress = progress > 0.0 ? progress : 0.0;

	for ( auto subscriber : subscribers )
		subscriber->progress_changed( progress );
}

void RoughNotifier::subscribe( const pISubscriber &subscriber )
{
	subscribers.insert( subscriber );
}

void RoughNotifier::notify_of_session_finished( )
{
	for ( auto subscriber : subscribers )
	{
		subscriber->progress_changed( 100.0 );
		subscriber->session_finished( );
	}
	subscribers.clear( );
}

void RoughNotifier::unsubscribe( const pISubscriber &subscriber )
{
	subscribers.erase( subscriber );//todo: check what if different ptrs to same object
}
