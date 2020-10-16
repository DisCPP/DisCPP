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
         * If the role is not found in cache, a DiscordObjectNotFound exception will be thrown.
         *
         * ```cpp
         *      discpp::Role role(client, 657246994997444614, guild);
         * ```
         *
         * @param[in] client The client.
         * @param[in] role_id The role id.
         * @param[in] guild The guild that has this role.
         *
         * @return discpp::Role, this is a constructor.
         */
		Role(discpp::Client* client, const Snowflake& role_id, Guild &guild);

        /**
         * @brief Constructs a discpp::Role object by parsing json.
         *
         * ```cpp
         *      discpp::Role role(json);
         * ```
         *
         * @param[in] client The client.
         * @param[in] json The json that makes up of role object.
         *
         * @return discpp::Role, this is a constructor.
         */
        Role(discpp::Client* client, rapidjson::Document& json);

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

        [[nodiscard]] std::string GetFormattedTime(CommonTimeFormat format_type = CommonTimeFormat::DEFAULT, const std::string& format_str = "", bool localtime = false) const noexcept {
            return this->id.GetFormattedTimestamp(format_type, format_str, localtime);
        }
        [[nodiscard]] time_t GetRawTime() const noexcept {
            return this->id.GetRawTime();
        }

		std::string name; /**< Name of the current role. */
		int color; /**< Color of the current role. */
		int position; /**< Position of the current role. */
		Permissions permissions; /**< PermissionOverwrites for the current role. */
	private:
	    unsigned char flags = 0b0;
	};
}

#endif