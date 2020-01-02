#ifndef DISCORDPP_MEMBER_H
#define DISCORDPP_MEMBER_H

#include "discord_object.h"
#include "user.h"

#include <vector>

namespace discord {
	class Role;

	class Member : DiscordObject{
	public:
		Member() = default;
		Member(snowflake id);
		Member(nlohmann::json json, snowflake guild_id);

		void ModifyMember(std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf, snowflake channel_id);

		discord::User user;
		snowflake guild_id; // TODO: Convert this to discord::Guild instead of snowflakes
		std::string nick;
		std::vector<discord::Role> roles;
		std::string joined_at; // TODO: Convert to iso8601Time
		std::string premium_since; // TODO: Convert to iso8601Time
		bool deaf;
		bool mute;
	};
}

#endif