#include "guild.h"

namespace discord {
	Role::Role(snowflake role_id, discord::Guild guild) : DiscordObject(role_id) {
		/**
		 * @brief Constructs a discord::Role object using a role id and a guild.
		 *
		 * This constructor searches the roles cache in the guild object to get the role object from.
		 *
		 * ```cpp
		 *      discord::Role role(657246994997444614, guild);
		 * ```
		 *
		 * @param[in] role_id The role id.
		 * @param[in] guild The guild that has this role.
		 *
		 * @return discord::Role, this is a constructor.
		 */

		std::unordered_map<snowflake, Role>::iterator it = guild.roles.find(role_id); 
		if (it != guild.roles.end()) {
			*this = it->second;
		}
	}

	Role::Role(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Role object by parsing json.
		 *
		 * ```cpp
		 *      discord::Role role(json);
		 * ```
		 *
		 * @param[in] json The json that makes up of role object.
		 *
		 * @return discord::Role, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		name = GetDataSafely<std::string>(json, "name");
		color = GetDataSafely<int>(json, "color");
		hoist = GetDataSafely<bool>(json, "hoist");
		position = GetDataSafely<int>(json, "position");
		permissions = (json.contains("permissions")) ? Permissions(PermissionType::ROLE, json["permissions"].get<int>()) : Permissions();
		managed = GetDataSafely<bool>(json, "manages");
		mentionable = GetDataSafely<bool>(json, "mentionable");
	}
}