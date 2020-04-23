#ifndef DISCPP_GUILD_DELETE_EVENT_H
#define DISCPP_GUILD_DELETE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildDeleteEvent : public Event {
	public:
		inline GuildDeleteEvent(discpp::Guild guild) : guild(guild) {}

		discpp::Guild guild;
	};
}

#endif