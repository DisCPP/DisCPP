#include "guild.h"
#include "client.h"
#include <climits>

namespace discpp {
	Member::Member(snowflake id, discpp::Guild guild) : discpp::DiscordObject(id) {
		/**
		 * @brief Constructs a discpp::Member object using its id and the guild that it is in.
		 *
		 * This constructor searches the guild's member cache to get a member object.
		 *
		 * ```cpp
		 *      discpp::Member member("222189653795667968", guild);
		 * ```
		 *
		 * @param[in] id The id of the member.
		 * @param[in] guild The guild containing the member.
		 *
		 * @return discpp::Member, this is a constructor.
		 */

		*this = guild.GetMember(id);

		/*auto it = std::find_if(discpp::globals::bot_instance->members.begin(), discpp::globals::bot_instance->members.end(),
		        [](std::unordered_map<snowflake, Member>::) {});

		if (it != discpp::globals::bot_instance->members.end()) {
			*this = it->second;
		}*/
	}

	Member::Member(rapidjson::Document& json, discpp::Guild guild) : guild_id(guild.id) {
		/**
		 * @brief Constructs a discpp::Member object by parsing json and stores the guild_id.
		 *
		 * ```cpp
		 *      discpp::Member member(json, 657246994997444614);
		 * ```
		 *
		 * @param[in] json The json that makes up of member object.
		 * @param[in] json guild_id The guild id.
		 *
		 * @return discpp::Member, this is a constructor.
		 */

		user = GetDiscppObject(json, "user", discpp::User());
		if (!user.id.empty()) id = user.id;
		nick = json["nick"].GetString();

        int highest_hiearchy = 0;
        rapidjson::Value::ConstMemberIterator itr = json.FindMember("roles");
		itr = json.FindMember("roles");
		if (itr != json.MemberEnd()) {
			for (auto& role : json["roles"].GetArray()) {
				rapidjson::Document role_json;
				role_json.CopyFrom(role, role_json.GetAllocator());
				discpp::Role r(static_cast<snowflake>(role_json.GetString()), guild);
				if (r.position > highest_hiearchy) {
					highest_hiearchy = r.position;
				}

				// Save permissions
				if (json["roles"][0] == role) {
					permissions.allow_perms.value = r.permissions.allow_perms.value;
					permissions.deny_perms.value = r.permissions.deny_perms.value;
				}
				else {
					permissions.allow_perms.value |= r.permissions.allow_perms.value;
					permissions.deny_perms.value |= r.permissions.deny_perms.value;
				}

				roles.push_back(r);
			}
		}

		if (guild.owner_id == this->id) {
            hierarchy = INT_MAX;
		} else {
            hierarchy = highest_hiearchy;
        }
		joined_at = json["joined_at"].GetString();
		premium_since = GetStringSafely(json, "premium_since");
		deaf = json["deaf"].GetBool();
		mute = json["mute"].GetBool();
		user.mention = "<@!" + id + ">";
	}

	void Member::ModifyMember(std::string nick, std::vector<discpp::Role> roles, bool mute, bool deaf, snowflake channel_id) {
		/**
		 * @brief Modifies this guild member.
		 *
		 * ```cpp
		 *      member.ModifyMember("Member nick", roles, true, false, 657246994997444614);
		 * ```
		 *
		 * @param[in] nick The new member nickname.
		 * @param[in] roles The new member role.
		 * @param[in] mute Whether or not the member is muted in voice channels.
		 * @param[in] deaf Whether or not the member is deafened in voice channels.
		 * @param[in] channel_id The voice channel to move them to if they're connected to one.
		 *
		 * @return void
		 */

		std::string json_roles = "[";
		for (discpp::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			}
			else {
				json_roles += ", \"" + role.id + "\"";
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

		cpr::Body body("{\"nick\": \"" + EscapeString(nick) + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "\"channel_id\": \"" + channel_id + "\"" + "}");
		SendPatchRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), guild_id, RateLimitBucketType::GUILD, body);
	}

	void Member::AddRole(discpp::Role role) {
		/**
		 * @brief Adds a role to a guild member.
		 *
		 * ```cpp
		 *      guild.AddRole(role);
		 * ```
		 *
		 * @param[in] role The role to add.
		 *
		 * @return void
		 */

		SendPutRequest(Endpoint("/guilds/" + guild_id + "/members/" + id + "/roles/" + role.id), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	void Member::RemoveRole(discpp::Role role) {
		/**
		 * @brief Removes a role to a guild member.
		 *
		 * ```cpp
		 *      guild.RemoveRole(role);
		 * ```
		 *
		 * @param[in] role The role to remove.
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/guilds/" + guild_id + "/members/" + id + "/roles/" + role.id), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
	}

	bool Member::IsBanned() {
		/**
		 * @brief Check if a member is banned.
		 *
		 * ```cpp
		 *      bool is_banned = guild.IsBanned(member);
		 * ```
		 *
		 * @return bool
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + guild_id + "/bans/" + id), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
		rapidjson::Value::ConstMemberIterator itr = result.FindMember("reason");
		return itr != result.MemberEnd();
	}

	bool Member::HasRole(discpp::Role role) {
		/**
		 * @brief Check if this member is a role.
		 *
		 * ```cpp
		 *      bool has_role = member.HasRole(role);
		 * ```
		 *
		 * @param[in] role The role to check if the member has it.
		 *
		 * @return bool
		 */

		return count_if(roles.begin(), roles.end(), [role](discpp::Role r) { return role.id == r.id; }) != 0;
	}

	bool Member::HasPermission(discpp::Permission perm) {
		/**
		 * @brief Check if this member has a permission. It will also check if it has the Administrator permission or is guild owner.
		 *
		 * ```cpp
		 *      bool has_perm = member.HasPermission(discpp::Permission::MANAGE_CHANNELS);
		 * ```
		 *
		 * @param[in] perm The permission to check that the member has.
		 *
		 * @return bool
		 */

		// Check if the member has the permission, has the admin permission, or is the guild owner.
		bool has_perm = permissions.allow_perms.HasPermission(perm) && !permissions.deny_perms.HasPermission(perm);
		has_perm = has_perm || (permissions.allow_perms.HasPermission(Permission::ADMINISTRATOR) && !permissions.deny_perms.HasPermission(Permission::ADMINISTRATOR));
		has_perm = has_perm || discpp::Guild(guild_id).owner_id == this->id;

		return has_perm;
	}
}