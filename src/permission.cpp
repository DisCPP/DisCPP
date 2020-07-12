#include "permission.h"
#include "utils.h"

namespace discpp {
	Permissions::Permissions(const PermissionType& permission_type, const int& byte_set) : permission_type(permission_type) {
		allow_perms = PermissionOverwrite(byte_set);
	}

	Permissions::Permissions(const discpp::JsonObject& json) {
		role_user_id = SnowflakeFromString(json["id"].GetString());
		permission_type = (json["type"].GetString() == "role") ? PermissionType::ROLE : PermissionType::MEMBER;
		allow_perms = PermissionOverwrite(json["allow"].GetInt());
		deny_perms = PermissionOverwrite(json["deny"].GetInt());
	}

    discpp::JsonObject Permissions::ToJson() {
		std::string str_type = (permission_type == PermissionType::ROLE) ? "role" : "member";

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document json;
        json.AddMember("id", role_user_id, json.GetAllocator());
        json.AddMember("type", str_type, json.GetAllocator());
        json.AddMember("allow", allow_perms.value, json.GetAllocator());
        json.AddMember("deny", deny_perms.value, json.GetAllocator());

        return discpp::JsonObject(json);
#elif SIMDJSON_BACKEND

#endif
	}

	bool PermissionOverwrite::HasPermission(const Permission& permission) {
		return (value & permission) == permission;
	}

	void PermissionOverwrite::AddPermission(const Permission& permission) {
		value |= permission;
	}
}