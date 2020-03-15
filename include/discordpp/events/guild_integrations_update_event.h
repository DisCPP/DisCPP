#ifndef DISCORDPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H
#define DISCORDPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildIntegrationsUpdateEvent : public Event {
	public:
		inline GuildIntegrationsUpdateEvent(discord::Guild guild) : guild(guild) {}

		discord::Guild guild;
	};
}

#endif