#ifndef DISCPP_PERMISSION_H
#define DISCPP_PERMISSION_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <unordered_map>
#include <stdexcept>

#include <rapidjson/document.h>

namespace discpp {
	typedef uint64_t snowflake;

	enum class PermissionType : int {
		ROLE,
		MEMBER
	};

	enum Permission : int {
		CREATE_INSTANT_INVITE = 0x00000001,
		KICK_MEMBERS = 0x00000002,
		BAN_MEMBERS = 0x00000004,
		ADMINISTRATOR = 0x00000008,
		MANAGE_CHANNELS = 0x00000010,
		MANAGE_GUILD = 0x00000020,
		ADD_REACTIONS = 0x00000040,
		VIEW_AUDIT_LOG = 0x00000080,
		READ_MESSAGES = 0x00000400,
		SEND_MESSAGES = 0x00000800,
		SEND_TTS_MESSAGES = 0x00001000,
		MANAGE_MESSAGES = 0x00002000,
		EMBED_LINKS = 0x00004000,
		ATTACH_FILES = 0x00008000,
		READ_MESSAGE_HISTORY = 0x00010000,
        MENTION_EVERYONE_HERE_ALL_ROLES = 0x00020000,
		USE_EXTERNAL_EMOJIS = 0x00040000,
		CONNECT = 0x00100000,
		SPEAK = 0x00200000,
		MUTE_MEMBERS = 0x00400000,
		DEAFEN_MEMBERS = 0x00800000,
		MOVE_MEMBERS = 0x01000000,
		USE_VAD = 0x02000000,
		PRIORITY_SPEAKER = 0x00000100,
		STREAM = 0x00000200,
		CHANGE_NICKNAME = 0x04000000,
		MANAGE_NICKNAMES = 0x08000000,
		MANAGE_ROLES = 0x10000000,
		MANAGE_WEBHOOKS = 0x20000000,
		MANAGE_EMOJIS = 0x40000000,
	};

	inline std::string PermissionToString(Permission perm) {
        std::unordered_map<Permission, std::string> permission_str_map = {
                {Permission::CREATE_INSTANT_INVITE, "CREATE_INSTANT_INVITE"},
                {Permission::KICK_MEMBERS, "KICK_MEMBERS"},
                {Permission::BAN_MEMBERS, "BAN_MEMBERS"},
                {Permission::ADMINISTRATOR, "ADMINISTRATOR"},
                {Permission::MANAGE_CHANNELS, "MANAGE_CHANNELS"},
                {Permission::MANAGE_GUILD, "MANAGE_GUILD"},
                {Permission::ADD_REACTIONS, "ADD_REACTIONS"},
                {Permission::VIEW_AUDIT_LOG, "VIEW_AUDIT_LOG"},
                {Permission::READ_MESSAGES, "READ_MESSAGES"},
                {Permission::SEND_MESSAGES, "SEND_MESSAGES"},
                {Permission::SEND_TTS_MESSAGES, "SEND_TTS_MESSAGES"},
                {Permission::MANAGE_MESSAGES, "MANAGE_MESSAGES"},
                {Permission::EMBED_LINKS, "EMBED_LINKS"},
                {Permission::ATTACH_FILES, "ATTACH_FILES"},
                {Permission::READ_MESSAGE_HISTORY, "READ_MESSAGE_HISTORY"},
                {Permission::MENTION_EVERYONE_HERE_ALL_ROLES, "MENTION_EVERYONE_HERE_ALL_ROLES"},
                {Permission::USE_EXTERNAL_EMOJIS, "USE_EXTERNAL_EMOJIS"},
                {Permission::CONNECT, "CONNECT"},
                {Permission::SPEAK, "SPEAK"},
                {Permission::MUTE_MEMBERS, "MUTE_MEMBERS"},
                {Permission::DEAFEN_MEMBERS, "DEAFEN_MEMBERS"},
                {Permission::MOVE_MEMBERS, "MOVE_MEMBERS"},
                {Permission::USE_VAD, "USE_VAD"},
                {Permission::PRIORITY_SPEAKER, "PRIORITY_SPEAKER"},
                {Permission::STREAM, "STREAM"},
                {Permission::CHANGE_NICKNAME, "CHANGE_NICKNAME"},
                {Permission::MANAGE_NICKNAMES, "MANAGE_NICKNAMES"},
                {Permission::MANAGE_ROLES, "MANAGE_ROLES"},
                {Permission::MANAGE_WEBHOOKS, "MANAGE_WEBHOOKS"},
                {Permission::MANAGE_EMOJIS, "MANAGE_EMOJIS"}
        };
	    return permission_str_map[perm];
	}

	class PermissionOverwrite {
	public:
		PermissionOverwrite() = default;

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
		PermissionOverwrite(const int& value) : value(value) {}

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
		bool HasPermission(const Permission& permission);

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
		void AddPermission(const Permission& permission);

		int value = 0;
	};

	class NoPermissionException : public std::runtime_error {
	public: 
		NoPermissionException (const Permission& req_perm) : std::runtime_error("Required permission " + PermissionToString(req_perm) + " not met.") {}
	};

	class NotGuildOwnerException : public std::runtime_error {
	public: 
		NotGuildOwnerException() : std::runtime_error("Ownership of guild requirement not met.") {}
	};

	class Permissions {
	public:
		Permissions() = default;

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
		Permissions(const PermissionType& permission_type, const int& byte_set);

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
		Permissions(rapidjson::Document& json);

        /**
         * @brief Converts this permissions object to json.
         *
         * ```cpp
         *      rapidjson::Document json = permissions.ToJson();
         * ```
         *
         * @return rapidjson::Document
         */
        rapidjson::Document ToJson();

		snowflake role_user_id;
		PermissionOverwrite allow_perms;
		PermissionOverwrite deny_perms;
		PermissionType permission_type;
	};
}

#endif