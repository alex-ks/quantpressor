#include "Stdafx.h"

#include "Notifications.h"

module_api::Notification::Notification( const std::wstring & sender, const std::wstring & message ) :
	sender( sender ), message( message )
{

}

module_api::Notification::~Notification( )
{

}

std::wstring module_api::Notification::get_message( ) const
{
	return message;
}

std::wstring module_api::Notification::get_sender( ) const
{
	return sender;
}
