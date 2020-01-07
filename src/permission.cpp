#include "permission.h"
#include "utils.h"

namespace discord {
	Permissions::Permissions(PermissionType permission_type, int byte_set) : permission_type(permission_type) {
		allow_perms = PermissionOverwrite(byte_set);
	}

	Permissions::Permissions(nlohmann::json json) {
		role_user_id = json["id"].get<snowflake>();
		permission_type = (json["type"] == "role") ? PermissionType::ROLE : PermissionType::MEMBER;
		allow_perms = PermissionOverwrite(json["allow"].get<int>());
		deny_perms = PermissionOverwrite(json["deny"].get<int>());
	}

	nlohmann::json Permissions::ToJson() {
		return nlohmann::json({
			{"id", role_user_id},
			{"type", (permission_type == PermissionType::ROLE) ? "role" : "member"},
			{"allow", allow_perms.value},
			{"deny", deny_perms.value}
		});
	}

	PermissionOverwrite::PermissionOverwrite(int value) : value(value) { }

	bool PermissionOverwrite::HasPermission(Permission permission) {
		return (value & permission) == permission;
	}

	void PermissionOverwrite::AddPermission(Permission permission) {
		value |= permission;
	}
}