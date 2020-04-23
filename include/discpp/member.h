#ifndef DISCPP_MEMBER_H
#define DISCPP_MEMBER_H

#include "discord_object.h"
#include "user.h"
#include "activity.h"

#include <vector>

namespace discpp {
	class Role;
	class Guild;

	class Member : public DiscordObject{
	public:
		Member() = default;
		Member(snowflake id);
		Member(nlohmann::json json, discpp::Guild guild);

		void ModifyMember(std::string nick, std::vector<discpp::Role> roles, bool mute, bool deaf, snowflake channel_id);
		void AddRole(discpp::Role role);
		void RemoveRole(discpp::Role role);
		bool IsBanned();
		bool HasRole(discpp::Role role);
		bool HasPermission(discpp::Permission perm);

		discpp::User user; /**< Discord user object */
		snowflake guild_id; /**< ID of the guild the current member is in */
		std::string nick; /**< Nickname of current member if it has one */
		std::vector<discpp::Role> roles; /**< Roles the current member has */
		std::string joined_at; // TODO: Convert to iso8601Time
		std::string premium_since; // TODO: Convert to iso8601Time
		bool deaf; /**< Whether or not the current member is deafened */
		bool mute; /**< Whether or not the current member is muted */
		discpp::Permissions permissions; /**< Guild permissions for the current member */
		discpp::Activity activity; /**< Activity for the current member */
		int hierarchy; /**< Role hierarchy for the current member */
	};
}

#endif