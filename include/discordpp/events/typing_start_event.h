#ifndef DISCORDPP_TYPING_START_EVENT_H
#define DISCORDPP_TYPING_START_EVENT_H

#include "event.h"
#include "User.h"
#include "channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class TypingStartEvent : public Event {
	public:
		inline TypingStartEvent(nlohmann::json json) : user(discord::User(json["user_id"].get<snowflake>())), channel(discord::Channel(json["guild_id"].get<snowflake>())), timestamp(json["timestamp"].get<int>()) {
			if (json.contains("guild_id")) channel.guild_id = json["guild_id"].get<snowflake>();
		}

		discord::User user;
		discord::Channel channel;
		int timestamp;
	};
}

#endif