#ifndef DISCORDPP_MEMBER_H
#define DISCORDPP_MEMBER_H

#include "discord_object.h"
#include "user.h"

namespace discord {
	class Member : DiscordObject{
	public:
		Member() = default;
		Member(snowflake id);
		Member(nlohmann::json json);

		discord::User user;
		std::string nick;
		std::vector<discord::Role> roles; // TODO: turn this into a vector of discord::Role instead of snowflakes
		std::string joined_at; // TODO: Convert to iso8601Time
		std::string premium_since; // TODO: Convert to iso8601Time
		bool deaf;
		bool mute;
	};
}

#endif