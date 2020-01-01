#ifndef DISCORDPP_ROLE_H
#define DISCORDPP_ROLE_H

#include "discord_object.h"
#include "permission.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Role : DiscordObject {
	public:
		Role() = default;
		Role(nlohmann::json json);

		snowflake id;
		std::string name;
		int color;
		bool hoist;
		int position;
		Permissions permissions;
		bool managed;
		bool mentionable;
	};
}

#endif