#ifndef DISCORDPP_EMOJI_H
#define DISCORDPP_EMOJI_H

#include "discord_object.h"
#include "user.h"
#include "role.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Guild;
	class User;

	class Emoji : DiscordObject {
	public:
		Emoji() = default;
		Emoji(std::string name, snowflake id);
		Emoji(discord::Guild guild, snowflake id);
		Emoji(nlohmann::json json);

		snowflake id;
		std::string name;
		std::vector<discord::Role> roles;
		discord::User user;
		bool require_colons;
		bool managed;
		bool animated;
	};
}

#endif