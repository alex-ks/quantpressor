#pragma once
#include <set>
#include "Notifications.h"

namespace igor
{
	class RoughNotifier : public module_api::INotifier
	{
	private:
		std::set<module_api::pISubscriber> subscribers;

	public:
		void notify( module_api::Notification notification ) override;
		void set_progress( double progress ) override;
		void notify_of_session_finished( ) override;

		void subscribe( const module_api::pISubscriber &subscriber ) override;
		void unsubscribe( const module_api::pISubscriber &subscriber ) override;
	};
}