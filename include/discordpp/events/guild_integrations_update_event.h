#ifndef DISCORDPP_GUILD_GUILD_INTEGRATIONS_UPDATE_EVENT_H
#define DISCORDPP_GUILD_GUILD_INTEGRATIONS_UPDATE_EVENT_H

#include "event.h"
#include "guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildGuildIntegrationsUpdateEvent : public Event {
	public:
		inline GuildGuildIntegrationsUpdateEvent(nlohmann::json json) : guild(discord::Guild(json)) {}

		discord::Guild guild;
	};
}

#endif