#ifndef DISCORDPP_GUILD_ROLE_DELETE_EVENT_H
#define DISCORDPP_GUILD_ROLE_DELETE_EVENT_H

#include "../event.h"
#include "../role.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildRoleDeleteEvent : public Event {
	public:
		inline GuildRoleDeleteEvent(discord::Role role) : role(role) {}

		discord::Role role;
	};
}

#endif