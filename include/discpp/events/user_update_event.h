#ifndef DISCPP_USER_UPDATE_EVENT_H
#define DISCPP_USER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class UserUpdateEvent : public Event {
	public:
		inline UserUpdateEvent(discpp::User user) : user(user) {}

		discpp::User user;
	};
}

#endif