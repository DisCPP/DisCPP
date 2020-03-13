#ifndef DISCORDPP_GUILD_UPDATE_EVENT_H
#define DISCORDPP_GUILD_UPDATE_EVENT_H

#include "event.h"
#include "guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildUpdateEvent : public Event {
	public:
		inline GuildUpdateEvent(nlohmann::json json) : guild(discord::Guild(json)) {}

		discord::Guild guild;
	};
}

#endif