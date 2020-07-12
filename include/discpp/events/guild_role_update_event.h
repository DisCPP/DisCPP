#ifndef DISCPP_GUILD_ROLE_UPDATE_EVENT_H
#define DISCPP_GUILD_ROLE_UPDATE_EVENT_H

#include "../event.h"
#include "../role.h"

namespace discpp {
	class GuildRoleUpdateEvent : public Event {
	public:
		inline GuildRoleUpdateEvent(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Role> role) : guild(guild), role(role) {}

        std::shared_ptr<discpp::Guild> guild;
		std::shared_ptr<discpp::Role> role;
	};
}

#endif