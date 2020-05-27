#ifndef DISCPP_MEMBER_H
#define DISCPP_MEMBER_H

#include "discord_object.h"
#include "user.h"
#include "presence.h"
#include "permission.h"

#include <vector>

namespace discpp {
	class Role;
	class Guild;

	class Member : public DiscordObject {
	public:
		Member() = default;
		Member(const snowflake& id, const discpp::Guild& guild);
		Member(rapidjson::Document& json, const discpp::Guild& guild);

		void ModifyMember(const std::string& nick, std::vector<discpp::Role>& roles, const bool& mute, const bool& deaf, const snowflake& channel_id);
		void AddRole(const discpp::Role& role);
		void RemoveRole(const discpp::Role& role);
		bool HasRole(const discpp::Role& role);
		bool HasPermission(const discpp::Permission& perm);
		bool IsBanned();

		bool IsDeafened();
		bool IsMuted();

		discpp::User user; /**< The user this guild member represents. */
		snowflake guild_id; /**< ID of the guild the current member is in. */
		std::string nick; /**< This users guild nickname. */
		std::unordered_map<discpp::snowflake, std::shared_ptr<discpp::Role>> roles; /**< Roles the current member has. */
        // TODO: Convert to iso8601Time
		std::string joined_at; /**< When the user joined the guild. */
        // TODO: Convert to iso8601Time
		std::string premium_since; /**< When the user started boosting the guild. */
		discpp::Permissions permissions; /**< Guild permissions for the current member. */
		discpp::Presence presence; /**< Presence for the current member. */
		int hierarchy; /**< Role hierarchy for the current member. */
	private:
	    char flags; /**< Internal use only. */
	};
}

#endif