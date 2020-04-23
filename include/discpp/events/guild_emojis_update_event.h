#ifndef DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H
#define DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildEmojisUpdateEvent : public Event {
	public:
		inline GuildEmojisUpdateEvent(discpp::Guild guild) : guild(guild) {}

		discpp::Guild guild;
	};
}

#endif