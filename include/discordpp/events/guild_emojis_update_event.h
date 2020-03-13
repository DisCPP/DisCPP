#ifndef DISCORDPP_GUILD_EMOJIS_UPDATE_EVENT_H
#define DISCORDPP_GUILD_EMOJIS_UPDATE_EVENT_H

#include "event.h"
#include "guild.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildEmojisUpdateEvent : public Event {
	public:
		inline GuildEmojisUpdateEvent(nlohmann::json json) : guild(discord::Guild(json)) {}

		discord::Guild guild;
	};
}

#endif