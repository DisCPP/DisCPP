#include "permission.h"
#include "utils.h"

namespace discpp {
	Permissions::Permissions(PermissionType permission_type, int byte_set) : permission_type(permission_type) {
		/**
		 * @brief Constructs a discpp::Permission object with its type and byte set.
		 *
		 * ```cpp
		 *      discpp::Permissions perms(type, 0);
		 * ```
		 *
		 * @param[in] permission_type The permission type.
		 * @param[in] byte_set The permissions byte set.
		 *
		 * @return discpp::Permissions, this is a constructor.
		 */

		allow_perms = PermissionOverwrite(byte_set);
	}

	Permissions::Permissions(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::Permissions object by parsing json.
		 *
		 * ```cpp
		 *      discpp::Permissions perms(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the Permissions object.
		 *
		 * @return discpp::Permissions, this is a constructor.
		 */

		role_user_id = json["id"].GetString();
		permission_type = (json["type"] == "role") ? PermissionType::ROLE : PermissionType::MEMBER;
		allow_perms = PermissionOverwrite(json["allow"].GetInt());
		deny_perms = PermissionOverwrite(json["deny"].GetInt());
	}

    rapidjson::Document& Permissions::ToJson() {
		/**
		 * @brief Converts this permissions object to json.
		 *
		 * ```cpp
		 *      rapidjson::Document json = permissions.ToJson();
		 * ```
		 *
		 * @return nlohmann::json
		 */

		std::string str_type = (permission_type == PermissionType::ROLE) ? "role" : "member";

        rapidjson::Document json;
        json.AddMember("id", role_user_id, json.GetAllocator());
        json.AddMember("type", str_type, json.GetAllocator());
        json.AddMember("allow", allow_perms.value, json.GetAllocator());
        json.AddMember("deny", deny_perms.value, json.GetAllocator());

		return json;
	}

	PermissionOverwrite::PermissionOverwrite(int value) : value(value) { 
		/**
		 * @brief Constructs a discpp::PermissionOverwrite object with its permission value.
		 *
		 * ```cpp
		 *      discpp::PermissionOverwrite permission_overwrite(0);
		 * ```
		 *
		 * @param[in] value The permission overwrite value.
		 *
		 * @return discpp::PermissionOverwrite, this is a constructor.
		 */
	}

	bool PermissionOverwrite::HasPermission(Permission permission) {
		/**
		 * @brief Checks if the permission overwrites has a permission.
		 *
		 * ```cpp
		 *      bool has_perm = permission_overwrite.HasPermission(permission);
		 * ```
		 *
		 * @param[in] permission The permission to check if this permission overwrite has.
		 *
		 * @return bool
		 */

		return (value & permission) == permission;
	}

	void PermissionOverwrite::AddPermission(Permission permission) {
		/**
		 * @brief Add a permission.
		 *
		 * ```cpp
		 *      permission_overwrite.AddPermission(permission);
		 * ```
		 *
		 * @param[in] permission The permission add to the permissions overwrite.
		 *
		 * @return void
		 */

		value |= permission;
	}
}