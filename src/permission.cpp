#include "permission.h"
#include "utils.h"

namespace discpp {
	Permissions::Permissions(const PermissionType& permission_type, const int& byte_set) : permission_type(permission_type) {
		allow_perms = PermissionOverwrite(byte_set);
	}

	Permissions::Permissions(rapidjson::Document& json) {
		role_user_id = SnowflakeFromString(json["id"].GetString());
		permission_type = (json["type"] == "role") ? PermissionType::ROLE : PermissionType::MEMBER;
		allow_perms = PermissionOverwrite(json["allow"].GetInt());
		deny_perms = PermissionOverwrite(json["deny"].GetInt());
	}

    rapidjson::Document Permissions::ToJson() {
		std::string str_type = (permission_type == PermissionType::ROLE) ? "role" : "member";

        rapidjson::Document json;
        json.AddMember("id", (uint64_t) role_user_id, json.GetAllocator());
        json.AddMember("type", str_type, json.GetAllocator());
        json.AddMember("allow", allow_perms.value, json.GetAllocator());
        json.AddMember("deny", deny_perms.value, json.GetAllocator());

		return json;
	}

	bool PermissionOverwrite::HasPermission(const Permission& permission) {
		return (value & permission) == permission;
	}

	void PermissionOverwrite::AddPermission(const Permission& permission) {
		value |= permission;
	}
}