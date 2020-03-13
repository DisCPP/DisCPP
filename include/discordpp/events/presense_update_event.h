#ifndef DISCORDPP_PRESENSE_UDPATE_EVENT_H
#define DISCORDPP_PRESENSE_UDPATE_EVENT_H

#include "../event.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class PresenseUpdateEvent : public Event {
	public:
		inline PresenseUpdateEvent(nlohmann::json json) : user(discord::User(json["user"])) {}
		inline PresenseUpdateEvent(discord::User user) : user(user) {}

		discord::User user;
	};
}

#endif