#ifndef DISCORDPP_GUILD_ROLE_UPDATE_EVENT_H
#define DISCORDPP_GUILD_ROLE_UPDATE_EVENT_H

#include "event.h"
#include "role.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildRoleUpdateEvent : public Event {
	public:
		inline GuildRoleUpdateEvent(nlohmann::json json) : role(discord::Role(json["role"])) {}
		inline GuildRoleUpdateEvent(discord::Role role) : role(role) {}

		discord::Role role;
	};
}

#endif