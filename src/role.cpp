#include "guild.h"

namespace discpp {
	Role::Role(snowflake role_id, discpp::Guild& guild) : DiscordObject(role_id) {
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

		auto it = guild.roles.find(role_id);
		if (it != guild.roles.end()) {
			*this = *it->second;
		}
	}

	Role::Role(rapidjson::Document& json) {
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

		id = SnowflakeFromString(json["id"].GetString());
		name = json["name"].GetString();
		color = json["color"].GetInt();
		hoist = json["hoist"].GetBool();
		position = json["position"].GetInt();
        permissions = Permissions(PermissionType::ROLE, json["permissions"].GetInt());
		managed = json["managed"].GetBool();
		mentionable = json["mentionable"].GetBool();
	}
}