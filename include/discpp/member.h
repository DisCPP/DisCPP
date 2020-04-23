#ifndef DISCPP_MEMBER_H
#define DISCPP_MEMBER_H

#include "discord_object.h"
#include "user.h"
#include "activity.h"

#include <vector>

namespace discord {
	class Role;
	class Guild;

	class Member : public DiscordObject{
	public:
		Member() = default;
		Member(snowflake id);
		Member(nlohmann::json json, discord::Guild guild);

		void ModifyMember(std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf, snowflake channel_id);
		void AddRole(discord::Role role);
		void RemoveRole(discord::Role role);
		bool IsBanned();
		bool HasRole(discord::Role role);
		bool HasPermission(discord::Permission perm);

		discord::User user; /**< Discord user object */
		snowflake guild_id; /**< ID of the guild the current member is in */
		std::string nick; /**< Nickname of current member if it has one */
		std::vector<discord::Role> roles; /**< Roles the current member has */
		std::string joined_at; // TODO: Convert to iso8601Time
		std::string premium_since; // TODO: Convert to iso8601Time
		bool deaf; /**< Whether or not the current member is deafened */
		bool mute; /**< Whether or not the current member is muted */
		discord::Permissions permissions; /**< Guild permissions for the current member */
		discord::Activity activity; /**< Activity for the current member */
		int hierarchy; /**< Role hierarchy for the current member */
	};
}

#endif