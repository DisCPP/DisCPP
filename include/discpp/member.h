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
		Member(const Snowflake& id, const discpp::Guild& guild);

        /**
         * @brief Constructs a discpp::Member object by parsing json and stores the guild_id.
         *
         * ```cpp
         *      discpp::Member member(json, 657246994997444614);
         * ```
         *
         * @param[in] json The json that makes up of member object.
         * @param[in] json guild The guild containing this member.
         *
         * @return discpp::Member, this is a constructor.
         */
		Member(rapidjson::Document& json, const discpp::Guild& guild);

        Member(const discpp::Member& member);
        Member operator=(const discpp::Member& mbr);

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
		void ModifyMember(const std::string& nick, std::vector<discpp::Role>& roles, const bool& mute, const bool& deaf, const Snowflake& channel_id);

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
         * @brief Check if this member has a role.
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

        /**
         * @brief Formats the joined at time to text.
         *
         * @return std::string
         */
        inline std::string GetFormattedJoinedAt() const {
            return FormatTime(this->joined_at);
        }

        /**
         * @brief Formats the premium since time to text.
         *
         * @return std::string
         */
        inline std::string GetFormattedPremiumSince() const {
            if (premium_since == 0) return "";
            return FormatTime(this->premium_since);
        }

        /**
         * @brief Returns true if the user has this role
         *
         * @return bool
         */
        bool HasRole(discpp::Snowflake role_id);

        /**
         * @brief Gets all permissions for the user from the roles and returns them.
         *
         * @return discpp::Permissions
         */
        discpp::Permissions GetPermissions();

        /**
         * @brief Gets role hierarchy for the member.
         *
         * @return int
         */
        int GetHierarchy();

        /**
         * @brief Returns member roles as objects instead of the snowflake vector.
         *
         * @return std::vector<std::shared_ptr<discpp::Role>>
         */
        std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Role>> GetRoles();

        /**
         * @brief Returns guild object.
         *
         * @return std::shared_ptr<discpp::Guild>
         */
        std::shared_ptr<discpp::Guild> GetGuild();

		discpp::User user; /**< The user this guild member represents. */
		discpp::Snowflake guild_id; /**< The ID of the guild this member is in. */
        std::string nick; /**< This members guild nickname. If the member has no nickname, its a nullptr. */
		time_t joined_at; /**< When the user joined the guild. */
        time_t premium_since; /**< When the user started boosting the guild. */
		std::unique_ptr<discpp::Presence> presence = nullptr; /**< Presence for the current member. If the member has no presence, its a nullptr. */
        std::vector<discpp::Snowflake> roles;
	private:
	    unsigned char flags;
	};
}

#endif