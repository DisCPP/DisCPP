#ifndef DISCORDPP_USER_UPDATE_EVENT_H
#define DISCORDPP_USER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class UserUpdateEvent : public Event {
	public:
		inline UserUpdateEvent(nlohmann::json json) : user(discord::User(json)) { }
		inline UserUpdateEvent(discord::User user) : user(user) {}

		discord::User user;
	};
}

#endif