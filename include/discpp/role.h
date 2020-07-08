#ifndef DISCPP_ROLE_H
#define DISCPP_ROLE_H

#include "discord_object.h"
#include "permission.h"

namespace discpp {
	class Guild;

	class Role : public DiscordObject {
	public:
		Role() = default;

        /**
         * @brief Constructs a discpp::Role object using a role id and a guild.
         *
         * This constructor searches the roles cache in the guild object to get the role object from.
         *
         * ```cpp
         *      discpp::Role role(657246994997444614, guild);
         * ```
         *
         * @param[in] role_id The role id.
         * @param[in] guild The guild that has this role.
         *
         * @return discpp::Role, this is a constructor.
         */
		Role(const Snowflake& role_id, const discpp::Guild& guild);

        /**
         * @brief Constructs a discpp::Role object by parsing json.
         *
         * ```cpp
         *      discpp::Role role(json);
         * ```
         *
         * @param[in] json The json that makes up of role object.
         *
         * @return discpp::Role, this is a constructor.
         */
        Role(rapidjson::Document& json);

        /**
         * @brief Returns if the role is hoist-able or not. Which means the role displays in member list.
         *
         * @return bool
         */
        bool IsHoistable() const;

        /**
         * @brief Returns if the role is manged by an integration.
         *
         * @return bool
         */
        bool IsManaged() const;

        /**
         * @brief Returns if the role is mentionable.
         *
         * @return bool
         */
        bool IsMentionable() const;

		std::string name; /**< Name of the current role. */
		int color; /**< Color of the current role. */
		int position; /**< Position of the current role. */
		Permissions permissions; /**< PermissionOverwrites for the current role. */
	private:
	    unsigned char flags;
	};
}

#endif