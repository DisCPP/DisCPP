#ifndef DISCORDPP_MEMBER_H
#define DISCORDPP_MEMBER_H

#include "discord_object.h"
#include "user.h"
#include "activity.h"

#include <vector>

namespace discord {
	class Role;

	class Member : public DiscordObject{
	public:
		Member() = default;
		Member(snowflake id);
		Member(nlohmann::json json, snowflake guild_id);

		void ModifyMember(std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf, snowflake channel_id);
		void AddRole(discord::Role role);
		void RemoveRole(discord::Role role);
		bool IsBanned();
		bool HasRole(discord::Role role);
		bool HasPermission(discord::Permission perm);

		discord::User user;
		snowflake guild_id;
		std::string nick;
		std::vector<discord::Role> roles;
		std::string joined_at; // TODO: Convert to iso8601Time
		std::string premium_since; // TODO: Convert to iso8601Time
		bool deaf;
		bool mute;
		discord::Permissions permissions;
		discord::Activity activity;
		std::string created_at;
	};
}

#endif