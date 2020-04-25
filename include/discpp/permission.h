#ifndef DISCPP_PERMISSION_H
#define DISCPP_PERMISSION_H

#include <unordered_map>
#include <nlohmann/json.hpp>

namespace discpp {
	typedef std::string snowflake;

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
		VIEW_CHANNEL = 0x00000400,
		SEND_MESSAGES = 0x00000800,
		SEND_TTS_MESSAGES = 0x00001000,
		MANAGE_MESSAGES = 0x00002000,
		EMBED_LINKS = 0x00004000,
		ATTACH_FILES = 0x00008000,
		READ_MESSAGE_HISTORY = 0x00010000,
		MENTION_EVERYONE = 0x00020000,
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
		MANAGE_EMOJIS = 0x40000000
	};
	
	std::unordered_map<Permission, std::string> PermissionMap = {
		{Permission::CREATE_INSTANT_INVITE, "CREATE_INSTANT_INVITE"},
		{Permission::KICK_MEMBERS, "KICK_MEMBERS"},
		{Permission::BAN_MEMBERS, "BAN_MEMBERS"},
		{Permission::ADMINISTRATOR, "ADMINISTRATOR"},
		{Permission::MANAGE_CHANNELS, "MANAGE_CHANNELS"},
		{Permission::MANAGE_GUILD, "MANAGE_GUILD"},
		{Permission::ADD_REACTIONS, "ADD_REACTIONS"},
		{Permission::VIEW_AUDIT_LOG, "VIEW_AUDIT_LOG"},
		{Permission::VIEW_CHANNEL, "VIEW_CHANNEL"},
		{Permission::SEND_MESSAGES, "SEND_MESSAGES"},
		{Permission::SEND_TTS_MESSAGES, "SEND_TTS_MESSAGES"},
		{Permission::MANAGE_MESSAGES, "MANAGE_MESSAGES"},
		{Permission::EMBED_LINKS, "EMBED_LINKS"},
		{Permission::ATTACH_FILES, "ATTACH_FILES"},
		{Permission::READ_MESSAGE_HISTORY, "READ_MESSAGE_HISTORY"},
		{Permission::MENTION_EVERYONE, "MENTION_EVERYONE"},
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

	inline std::string PermissionToString(Permission perm) {
		return PermissionMap[perm];
	}

	class PermissionOverwrite {
	public:
		PermissionOverwrite() = default;
		PermissionOverwrite(int value);

		bool HasPermission(Permission permission);
		void AddPermission(Permission permission);

		int value = 0;
	};

	class NoPermissionException : public std::runtime_error {
	public: 
		NoPermissionException (Permission reqPerm) : std::runtime_error("Required permission " + PermissionToString(reqPerm) + " not met.") {}
	};

	class NotGuildOwner : public std::runtime_error {
	public: 
		NotGuildOwner() : std::runtime_error("Ownership of guild requirement not met.") {}
	};

	class Permissions {
	public:
		Permissions() = default;
		Permissions(PermissionType permission_type, int byte_set);
		Permissions(nlohmann::json json);
		nlohmann::json ToJson();

		snowflake role_user_id;
		PermissionOverwrite allow_perms;
		PermissionOverwrite deny_perms;
		PermissionType permission_type;
	};
}

#endif