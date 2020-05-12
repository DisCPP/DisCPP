#ifndef DISCPP_GUILD_DELETE_EVENT_H
#define DISCPP_GUILD_DELETE_EVENT_H

#include "../event.h"
#include "../guild.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildDeleteEvent : public Event {
	public:
		inline GuildDeleteEvent(std::shared_ptr<discpp::Guild> guild) : guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;
	};
}

#endif