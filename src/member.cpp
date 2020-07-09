#include "member.h"
#include "guild.h"
#include "client.h"
#include "role.h"

#include <climits>

namespace discpp {
	Member::Member(const Snowflake& id, discpp::Guild& guild, bool can_request) {
		*this = *guild.GetMember(id, can_request);
	}

	Member::Member(rapidjson::Document& json, const discpp::Guild& guild) : guild_id(guild.id) {
		user = ConstructDiscppObjectFromJson(json, "user", discpp::User());
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

				roles.emplace_back(r->id);
			}
		}
        joined_at = ContainsNotNull(json, "joined_at") ? TimeFromDiscord(json["joined_at"].GetString()) : 0;
        premium_since = ContainsNotNull(json, "premium_since") ? TimeFromDiscord(json["premium_since"].GetString()) : 0;
		if (GetDataSafely<bool>(json, "deaf")) {
		    flags |= 0b1;
		}
		if (GetDataSafely<bool>(json, "mute")) {
            flags |= 0b10;
		}
		if (discpp::ContainsNotNull(json, "presence")) {
            rapidjson::Document json_presence;
            json_presence.CopyFrom(json["presence"], json_presence.GetAllocator());

            presence = std::make_unique<discpp::Presence>(json_presence);
		}
	}

	bool Member::IsDeafened() {
	    return (flags & 0b1) == 0b1;
	}

	bool Member::IsMuted() {
        return (flags & 0b10) == 0b10;
	}

	void Member::ModifyMember(const std::string& nick, std::vector<discpp::Role>& roles, const bool mute, const bool deaf, const Snowflake& channel_id) {
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
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(this->user.id) + "/members/" + std::to_string(user.id)), DefaultHeaders({ { "Content-Type", "application/json" } }), guild_id, RateLimitBucketType::GUILD, body);
	}

	void Member::AddRole(const discpp::Role& role) {
		SendPutRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/members/" + std::to_string(user.id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	void Member::RemoveRole(const discpp::Role& role) {
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/members/" + std::to_string(user.id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	bool Member::IsBanned() {

		std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/bans/" + std::to_string(user.id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
		rapidjson::Value::ConstMemberIterator itr = result->FindMember("reason");
		return itr != result->MemberEnd();
	}

	bool Member::HasRole(const discpp::Role& role) {
	    auto roles_ptrs = GetRoles();
		return std::any_of(roles_ptrs.begin(), roles_ptrs.end(), [role](std::pair<Snowflake, std::shared_ptr<Role>> r) { return role.id == r.second->id; }) != 0;
	}

    bool Member::HasRole(discpp::Snowflake role_id) {
        auto roles_ptrs = GetRoles();
        return std::any_of(roles_ptrs.begin(), roles_ptrs.end(), [role_id](std::pair<Snowflake, std::shared_ptr<Role>> r) { return role_id == r.second->id; }) != 0;
    }

	bool Member::HasPermission(const discpp::Permission& perm) {
        discpp::Permissions permissions = GetPermissions();

		// Check if the member has the permission, has the admin permission, or is the guild owner.
		bool has_perm = permissions.allow_perms.HasPermission(perm) && !permissions.deny_perms.HasPermission(perm);
		has_perm = has_perm || (permissions.allow_perms.HasPermission(Permission::ADMINISTRATOR) && !permissions.deny_perms.HasPermission(Permission::ADMINISTRATOR));
		has_perm = has_perm || discpp::Guild(guild_id).owner_id == user.id;

		return has_perm;
	}

    discpp::Permissions Member::GetPermissions() {
        discpp::Permissions permissions;

        std::shared_ptr<discpp::Guild> guild = GetGuild();
        for (auto const& role : roles) {
            auto role_ptr = guild->GetRole(role);
            if (role == roles.front()) {
                permissions.allow_perms.value = role_ptr->permissions.allow_perms.value;
                permissions.deny_perms.value = role_ptr->permissions.deny_perms.value;
            } else {
                permissions.allow_perms.value |= role_ptr->permissions.allow_perms.value;
                permissions.deny_perms.value |= role_ptr->permissions.deny_perms.value;
            }
        }

        return permissions;
    }

    int Member::GetHierarchy() {
	    std::shared_ptr<discpp::Guild> guild = GetGuild();
        if (guild->owner_id == user.id) {
            return INT_MAX;
        } else {
            int highest_hiearchy = 0;
            for (auto& role : roles) {
                auto r_ptr = guild->GetRole(role);
                if (r_ptr->position > highest_hiearchy) {
                    highest_hiearchy = r_ptr->position;
                }
            }

            return highest_hiearchy;
        }
    }

    Member::Member(const Member &member) {
        this->user = member.user;
        this->guild_id = member.guild_id;
        this->nick = member.nick;

        this->roles = member.roles;
        this->joined_at = member.joined_at;
        this->premium_since = member.premium_since;

        if (member.presence != nullptr) {
            this->presence = std::make_unique<discpp::Presence>(*member.presence);
        }

        this->flags = member.flags;
    }

    Member Member::operator=(const discpp::Member& mbr) {
        return std::move(Member(mbr));
    }

    std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Role>> Member::GetRoles() {
        std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Role>> r;

        std::shared_ptr<discpp::Guild> guild = GetGuild();
	    for (auto const& role : roles) {
	        auto r_ptr = guild->GetRole(role);
            r.emplace(role, r_ptr);
	    }

        return r;
    }

    std::vector<std::shared_ptr<discpp::Role>> Member::GetSortedRoles() {
        std::vector<std::shared_ptr<discpp::Role>> tmp;

        std::shared_ptr<discpp::Guild> guild = GetGuild();
        for (auto const& role : roles) {
            auto r_ptr = guild->GetRole(role);
            tmp.push_back(r_ptr);
        }

        std::sort(tmp.begin(), tmp.end(), [](std::shared_ptr<discpp::Role> x, std::shared_ptr<discpp::Role> y) {
            return x->position < y->position;
        });

        return tmp;
    }

    std::shared_ptr<discpp::Role> Member::GetHighestRole(const bool isHoistable) {
        std::vector<std::shared_ptr<discpp::Role>> rolelist = this->GetSortedRoles();
	    std::shared_ptr<discpp::Role> role;
        if (isHoistable) {
	        for (auto tmp : rolelist) {
                if (tmp->IsHoistable()) {
                    role = tmp;
                    break;
                }
	        }
	    } else {
            role = rolelist[0];
        }
        return role;
	}

    std::shared_ptr<discpp::Guild> Member::GetGuild() {
        return discpp::globals::client_instance->cache.GetGuild(guild_id);
    }
}