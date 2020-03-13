#ifndef DISCORDPP_PRECENSE_UDPATE_EVENT_H
#define DISCORDPP_PRECENSE_UDPATE_EVENT_H

#include "event.h"
#include "user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class PrecenseUpdateEvent : public Event {
	public:
		inline PrecenseUpdateEvent(nlohmann::json json) : user(discord::User(json["user"])) {}

		discord::User user;
	};
}

#endif