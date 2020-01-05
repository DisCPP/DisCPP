#include "guild.h"
#include "role.h"
#include "utils.h"

namespace discord {
	Role::Role(snowflake role_id, discord::Guild guild) {
		auto role = std::find_if(guild.roles.begin(), guild.roles.end(), [role_id](discord::Role r) { return role_id == r.id; });

		if (role != guild.roles.end()) {
			*this = *role;
		}
	}

	Role::Role(nlohmann::json json) {
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