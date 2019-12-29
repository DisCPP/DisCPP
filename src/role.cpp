#include "role.h"
#include "utils.h"

namespace discord {
	Role::Role(nlohmann::json json) {
		id = json["id"].get<snowflake>();
		name = json["name"];
		color = json["color"].get<int>();
		hoist = json["hoist"].get<bool>();
		position = json["position"].get<int>();
		permissions = Permissions(PermissionType::ROLE, json["permissions"].get<int>());
		managed = json["managed"].get<bool>();
		mentionable = json["mentionable"].get<bool>();
	}
}