#include "guild.h"
#include "client.h"
#include <climits>
#include <discpp/member.h>


namespace discpp {
	Member::Member(const Snowflake& id, const discpp::Guild& guild) : discpp::DiscordObject(id) {
		*this = *guild.GetMember(id);
	}

	Member::Member(rapidjson::Document& json, const discpp::Guild& guild) : guild_id(guild.id) {
		user = ConstructDiscppObjectFromJson(json, "user", discpp::User());
		if (user.id != 0) id = user.id;
		nick = GetDataSafely<std::string>(json, "nick");

        int highest_hiearchy = 0;
		if (ContainsNotNull(json, "roles")) {
			for (auto& role : json["roles"].GetArray()) {
				rapidjson::Document role_json;
				role_json.CopyFrom(role, role_json.GetAllocator());

				std::shared_ptr<discpp::Role> r = guild.GetRole(SnowflakeFromString(role_json.GetString()));

				if (r->position > highest_hiearchy) {
					highest_hiearchy = r->position;
				}

				roles.emplace_back(r);
			}
		}

		if (guild.owner_id == this->id) {
            hierarchy = INT_MAX;
		} else {
            hierarchy = highest_hiearchy;
        }

		joined_at = TimeFromDiscord(GetDataSafely<std::string>(json, "joined_at"));
		std::string prm_since = GetDataSafely<std::string>(json, "premium_since");
		if (prm_since != "") premium_since = TimeFromDiscord(prm_since);
		if (GetDataSafely<bool>(json, "deaf")) {
		    flags |= 0b1;
		}
		if (GetDataSafely<bool>(json, "mute")) {
		    flags |= 0b10;
		}
		presence = ConstructDiscppObjectFromJson(json, "presence", discpp::Presence());
	}

	bool Member::IsDeafened() {
	    return (flags & 0b1) == 0b1;
	}

	bool Member::IsMuted() {
        return (flags & 0b10) == 0b10;
	}

	void Member::ModifyMember(const std::string& nick, std::vector<discpp::Role>& roles, const bool& mute, const bool& deaf, const Snowflake& channel_id) {
		std::string json_roles = "[";
		for (discpp::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + std::to_string(role.id) + "\"";
			}
			else {
				json_roles += ", \"" + std::to_string(role.id) + "\"";
			}
		}
		json_roles += "]";

		// Update permissions variable.
		discpp::Permissions permissions;
		if (roles.size() != 0) {
			permissions.allow_perms.value = roles.front().permissions.allow_perms.value;
			permissions.deny_perms.value = roles.front().permissions.deny_perms.value;
			roles.erase(roles.begin());

			for (discpp::Role role : roles) {
				permissions.allow_perms.value |= role.permissions.allow_perms.value;
				permissions.deny_perms.value |= role.permissions.deny_perms.value;
			}
		}

		cpr::Body body("{\"nick\": \"" + EscapeString(nick) + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "\"channel_id\": \"" + std::to_string(channel_id) + "\"" + "}");
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders({ { "Content-Type", "application/json" } }), guild_id, RateLimitBucketType::GUILD, body);
	}

	void Member::AddRole(const discpp::Role& role) {
		SendPutRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/members/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	void Member::RemoveRole(const discpp::Role& role) {
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/members/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	bool Member::IsBanned() {

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/bans/" + std::to_string(id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
		rapidjson::Value::ConstMemberIterator itr = result.FindMember("reason");
		return itr != result.MemberEnd();
	}

	bool Member::HasRole(const discpp::Role& role) {
		return std::any_of(roles.begin(), roles.end(), [role](std::shared_ptr<discpp::Role> r) { return role.id == r->id; }) != 0;
	}

	bool Member::HasPermission(const discpp::Permission& perm) {
        discpp::Permissions permissions = GetPermissions();

		// Check if the member has the permission, has the admin permission, or is the guild owner.
		bool has_perm = permissions.allow_perms.HasPermission(perm) && !permissions.deny_perms.HasPermission(perm);
		has_perm = has_perm || (permissions.allow_perms.HasPermission(Permission::ADMINISTRATOR) && !permissions.deny_perms.HasPermission(Permission::ADMINISTRATOR));
		has_perm = has_perm || discpp::Guild(guild_id).owner_id == this->id;

		return has_perm;
	}

    bool Member::HasRole(discpp::Snowflake role_id) {
	    return std::any_of(roles.begin(), roles.end(), [role_id](std::shared_ptr<discpp::Role> role) { return role->id == role_id; });
    }

    discpp::Permissions Member::GetPermissions() {
        discpp::Permissions permissions;
        for (auto const& role : roles) {
            if (role == roles.front()) {
                permissions.allow_perms.value = role->permissions.allow_perms.value;
                permissions.deny_perms.value = role->permissions.deny_perms.value;
            } else {
                permissions.allow_perms.value |= role->permissions.allow_perms.value;
                permissions.deny_perms.value |= role->permissions.deny_perms.value;
            }
        }

        return permissions;
    }
}