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

	Permissions::Permissions(nlohmann::json json) {
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

		role_user_id = json["id"].get<snowflake>();
		permission_type = (json["type"] == "role") ? PermissionType::ROLE : PermissionType::MEMBER;
		allow_perms = PermissionOverwrite(json["allow"].get<int>());
		deny_perms = PermissionOverwrite(json["deny"].get<int>());
	}

	nlohmann::json Permissions::ToJson() {
		/**
		 * @brief Converts this permissions object to json.
		 *
		 * ```cpp
		 *      nlohmann::json json = permissions.ToJson();
		 * ```
		 *
		 * @return nlohmann::json
		 */

		return nlohmann::json({
			{"id", role_user_id},
			{"type", (permission_type == PermissionType::ROLE) ? "role" : "member"},
			{"allow", allow_perms.value},
			{"deny", deny_perms.value}
		});
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