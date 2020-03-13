#ifndef DISCORDPP_GUILD_ROLE_CREATE_EVENT_H
#define DISCORDPP_GUILD_ROLE_CREATE_EVENT_H

#include "../event.h"
#include "../role.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildRoleCreateEvent : public Event {
	public:
		inline GuildRoleCreateEvent(nlohmann::json json) : role(discord::Role(json["role"])) {}
		inline GuildRoleCreateEvent(discord::Role role) : role(role) {}

		discord::Role role;
	};
}

#endif