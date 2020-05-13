#ifndef DISCPP_GUILD_ROLE_CREATE_EVENT_H
#define DISCPP_GUILD_ROLE_CREATE_EVENT_H

#include "../event.h"
#include "../role.h"



namespace discpp {
	class GuildRoleCreateEvent : public Event {
	public:
		inline GuildRoleCreateEvent(discpp::Role role) : role(role) {}

		discpp::Role role;
	};
}

#endif