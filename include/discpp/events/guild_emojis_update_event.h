#ifndef DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H
#define DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildEmojisUpdateEvent : public Event {
	public:
		inline GuildEmojisUpdateEvent(std::shared_ptr<discpp::Guild> guild) : guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;
	};
}

#endif