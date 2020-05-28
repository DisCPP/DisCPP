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

        /**
         * @brief Constructs a discpp::Member object using its id and the guild that it is in.
         *
         * This constructor searches the guild's member cache to get a member object.
         *
         * ```cpp
         *      discpp::Member member("222189653795667968", guild);
         * ```
         *
         * @param[in] id The id of the member.
         * @param[in] guild The guild containing the member.
         *
         * @return discpp::Member, this is a constructor.
         */
		Member(const snowflake& id, const discpp::Guild& guild);

        /**
         * @brief Constructs a discpp::Member object by parsing json and stores the guild_id.
         *
         * ```cpp
         *      discpp::Member member(json, 657246994997444614);
         * ```
         *
         * @param[in] json The json that makes up of member object.
         * @param[in] json guild_id The guild id.
         *
         * @return discpp::Member, this is a constructor.
         */
		Member(rapidjson::Document& json, const discpp::Guild& guild);

        /**
         * @brief Modifies this guild member.
         *
         * ```cpp
         *      member.ModifyMember("Member nick", roles, true, false, 657246994997444614);
         * ```
         *
         * @param[in] nick The new member nickname.
         * @param[in] roles The new member role.
         * @param[in] mute Whether or not the member is muted in voice channels.
         * @param[in] deaf Whether or not the member is deafened in voice channels.
         * @param[in] channel_id The voice channel to move them to if they're connected to one.
         *
         * @return void
         */
		void ModifyMember(const std::string& nick, std::vector<discpp::Role>& roles, const bool& mute, const bool& deaf, const snowflake& channel_id);

        /**
         * @brief Adds a role to a guild member.
         *
         * ```cpp
         *      guild.AddRole(role);
         * ```
         *
         * @param[in] role The role to add.
         *
         * @return void
         */
		void AddRole(const discpp::Role& role);

        /**
         * @brief Removes a role to a guild member.
         *
         * ```cpp
         *      guild.RemoveRole(role);
         * ```
         *
         * @param[in] role The role to remove.
         *
         * @return void
         */
		void RemoveRole(const discpp::Role& role);

        /**
         * @brief Check if this member is a role.
         *
         * ```cpp
         *      bool has_role = member.HasRole(role);
         * ```
         *
         * @param[in] role The role to check if the member has it.
         *
         * @return bool
         */
		bool HasRole(const discpp::Role& role);

        /**
         * @brief Check if this member has a permission. It will also check if it has the Administrator permission or is guild owner.
         *
         * ```cpp
         *      bool has_perm = member.HasPermission(discpp::Permission::MANAGE_CHANNELS);
         * ```
         *
         * @param[in] perm The permission to check that the member has.
         *
         * @return bool
         */
		bool HasPermission(const discpp::Permission& perm);

        /**
         * @brief Check if a member is banned.
         *
         * ```cpp
         *      bool is_banned = guild.IsBanned(member);
         * ```
         *
         * @return bool
         */
		bool IsBanned();

        /**
         * @brief Check if this member is deafened.
         *
         * ```cpp
         *      bool is_deafened = member.IsDeafened();
         * ```
         *
         * @return bool
         */
		bool IsDeafened();

        /**
         * @brief Check if this member is muted.
         *
         * ```cpp
         *      bool is_muted = member.IsMuted();
         * ```
         *
         * @return bool
         */
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