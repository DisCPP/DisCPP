#ifndef DISCORDPP_GUILD_EMOJIS_UPDATE_EVENT_H
#define DISCORDPP_GUILD_EMOJIS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildEmojisUpdateEvent : public Event {
	public:
		inline GuildEmojisUpdateEvent(discord::Guild guild) : guild(guild) {}

		discord::Guild guild;
	};
}

#endif