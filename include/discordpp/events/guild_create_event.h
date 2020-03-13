#ifndef DISCORDPP_GUILD_CREATE_EVENT_H
#define DISCORDPP_GUILD_CREATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildCreateEvent : public Event {
	public:
		inline GuildCreateEvent(nlohmann::json json) : guild(discord::Guild(json)) {}
		inline GuildCreateEvent(discord::Guild guild) : guild(guild) {}

		discord::Guild guild;
	};
}

#endif