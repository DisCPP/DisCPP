#ifndef DISCORDPP_GUILD_DELETE_EVENT_H
#define DISCORDPP_GUILD_DELETE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildDeleteEvent : public Event {
	public:
		inline GuildDeleteEvent(discord::Guild guild) : guild(guild) {}

		discord::Guild guild;
	};
}

#endif