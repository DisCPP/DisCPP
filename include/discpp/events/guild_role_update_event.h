#ifndef DISCPP_GUILD_ROLE_UPDATE_EVENT_H
#define DISCPP_GUILD_ROLE_UPDATE_EVENT_H

#include "../event.h"
#include "../role.h"



namespace discpp {
	class GuildRoleUpdateEvent : public Event {
	public:
		inline GuildRoleUpdateEvent(discpp::Role role) : role(role) {}

		discpp::Role role;
	};
}

#endif