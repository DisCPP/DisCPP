#include "permission.h"
#include "utils.h"

namespace discpp {
	Permissions::Permissions(const PermissionType& permission_type, const int& byte_set) : permission_type(permission_type) {
		allow_perms = PermissionOverwrite(byte_set);
	}

	Permissions::Permissions(rapidjson::Document& json) {
		role_user_id = Snowflake(json["id"].GetString());
		// For some reason discord thought they should have string-serialized numbers in audit log options...
		if (json["type"].IsString()) {
			permission_type = (PermissionType) std::stoi(json["type"].GetString());
		} else {
			permission_type = (PermissionType) json["type"].GetInt();
		}
		allow_perms = PermissionOverwrite(std::stoi(json["allow"].GetString()));
		deny_perms = PermissionOverwrite(std::stoi(json["deny"].GetString()));
	}

    rapidjson::Document Permissions::ToJson() {
		std::string str_type = (permission_type == PermissionType::ROLE) ? "role" : "member";

        rapidjson::Document json;
        json.AddMember("id", (uint64_t) role_user_id, json.GetAllocator());
        json.AddMember("type", str_type, json.GetAllocator());
        json.AddMember("allow", std::to_string(allow_perms.value), json.GetAllocator());
        json.AddMember("deny", std::to_string(deny_perms.value), json.GetAllocator());

		return json;
	}

	bool PermissionOverwrite::HasPermission(const Permission& permission) {
		return (value & permission) == permission;
	}

	void PermissionOverwrite::AddPermission(const Permission& permission) {
		value |= permission;
	}
}