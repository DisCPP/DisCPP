#include "guild.h"
#include "role.h"
#include "utils.h"

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

		auto role = std::find_if(guild.roles.begin(), guild.roles.end(), [role_id](discord::Role r) { return role_id == r.id; });

		if (role != guild.roles.end()) {
			*this = *role;
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