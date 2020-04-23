#ifndef DISCPP_GUILD_UPDATE_EVENT_H
#define DISCPP_GUILD_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildUpdateEvent : public Event {
	public:
		inline GuildUpdateEvent(discpp::Guild guild) : guild(guild) {}

		discpp::Guild guild;
	};
}

#endif