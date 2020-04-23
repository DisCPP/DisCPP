#ifndef DISCPP_GUILD_CREATE_EVENT_H
#define DISCPP_GUILD_CREATE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildCreateEvent : public Event {
	public:
		inline GuildCreateEvent(discpp::Guild guild) : guild(guild) {}

		discpp::Guild guild;
	};
}

#endif