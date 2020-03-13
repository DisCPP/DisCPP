#ifndef DISCORDPP_GUILD_CREATE_EVENT_H
#define DISCORDPP_GUILD_CREATE_EVENT_H

#include "event.h"
#include "guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildCreateEvent : public Event {
	public:
		inline ChannelCreateEvent(nlohmann::json json) : guild(discord::Guild(json)) {}

		discord::Guild guild;
	};
}

#endif