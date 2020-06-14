#include "role.h"
#include "guild.h"

namespace discpp {
	Role::Role(const Snowflake& role_id, const discpp::Guild& guild) : DiscordObject(role_id) {
		auto it = guild.roles.find(role_id);
		if (it != guild.roles.end()) {
			*this = *it->second;
		}
	}

	Role::Role(rapidjson::Document& json) {
		id = SnowflakeFromString(json["id"].GetString());
		name = json["name"].GetString();
		color = json["color"].GetInt();
		hoist = json["hoist"].GetBool();
		position = json["position"].GetInt();
        permissions = Permissions(PermissionType::ROLE, json["permissions"].GetInt());
		managed = json["managed"].GetBool();
		mentionable = json["mentionable"].GetBool();
	}
}