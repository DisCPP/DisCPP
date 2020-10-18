#include "role.h"
#include "guild.h"

namespace discpp {
	Role::Role(discpp::Client* client, const Snowflake& role_id, Guild &guild) : DiscordObject(client, role_id) {
	    *this = *guild.GetRole(role_id);
	}

	Role::Role(discpp::Client* client, rapidjson::Document& json) : DiscordObject(client) {
		id = Snowflake(json["id"].GetString());
		name = json["name"].GetString();
		color = json["color"].GetInt();
        if (GetDataSafely<bool>(json, "hoist")) {
            flags |= 0b1;
        }
		position = json["position"].GetInt();
        permissions = Permissions(PermissionType::ROLE, json["permissions"].GetInt());
        if (GetDataSafely<bool>(json, "managed")) {
            flags |= 0b10;
        }
        if (GetDataSafely<bool>(json, "mentionable")) {
            flags |= 0b100;
        }
	}

    bool Role::IsHoistable() const {
        return (flags & 0b1) == 0b1;
    }

    bool Role::IsManaged() const {
        return (flags & 0b10) == 0b10;
    }

    bool Role::IsMentionable() const {
        return (flags & 0b100) == 0b100;
    }
}