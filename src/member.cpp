#include "guild.h"
#include "role.h"
#include "member.h"
#include "utils.h"
#include "bot.h"

namespace discord {
	Member::Member(snowflake id) : discord::DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Member object using its id.
		 *
		 * This constructor searches the member cache to get a member object.
		 *
		 * ```cpp
		 *      discord::Member member(657246994997444614);
		 * ```
		 *
		 * @param[in] id The id of the member.
		 *
		 * @return discord::Member, this is a constructor.
		 */

		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			*this = *member;
		}
	}

	Member::Member(nlohmann::json json, snowflake guild_id) : guild_id(guild_id) {
		/**
		 * @brief Constructs a discord::Member object by parsing json and stores the guild_id.
		 *
		 * ```cpp
		 *      discord::Member member(json, 657246994997444614);
		 * ```
		 *
		 * @param[in] json The json that makes up of member object.
		 * @param[in] json guild_id The guild id.
		 *
		 * @return discord::Member, this is a constructor.
		 */

		if (json.contains("user")) {
			user = discord::User(json["user"]);
			id = user.id;
		} else {
			user = discord::User();
		}
		
		nick = GetDataSafely<std::string>(json, "nick");
		discord::Guild guild(guild_id);
		if (json.contains("roles")) {
			discord::Permissions permissions;
			for (auto& role : json["roles"]) {
				discord::Role r(role, guild);

				// Save permissions
				if (json["roles"][0] == role) {
					permissions.allow_perms.value = r.permissions.allow_perms.value;
					permissions.deny_perms.value = r.permissions.deny_perms.value;
				} else {
					permissions.allow_perms.value |= r.permissions.allow_perms.value;
					permissions.deny_perms.value |= r.permissions.deny_perms.value;
				}

				roles.push_back(r);
			}

			this->permissions = permissions;
		}
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		premium_since = GetDataSafely<std::string>(json, "premium_since");
		deaf = GetDataSafely<bool>(json, "deaf");
		mute = GetDataSafely<bool>(json, "mute");
	}

	void Member::ModifyMember(std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf, snowflake channel_id) {
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
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			}
			else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		// Update permissions variable.
		discord::Permissions permissions;
		if (roles.size() != 0) {
			permissions.allow_perms.value = roles.front().permissions.allow_perms.value;
			permissions.deny_perms.value = roles.front().permissions.deny_perms.value;
			roles.erase(roles.begin());

			for (discord::Role role : roles) {
				permissions.allow_perms.value |= role.permissions.allow_perms.value;
				permissions.deny_perms.value |= role.permissions.deny_perms.value;
			}
		}

		cpr::Body body("{\"nick\": \"" + EscapeString(nick) + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "\"channel_id\": \"" + channel_id + "\"" + "}");
		SendPatchRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);
	}

	void Member::AddRole(discord::Role role) {
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

	void Member::RemoveRole(discord::Role role) {
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

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/bans/%", guild_id, id), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
		return result.contains("reason");
	}

	bool Member::HasRole(discord::Role role) {
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

		return count_if(roles.begin(), roles.end(), [role](discord::Role r) { return role.id == r.id; }) != 0;
	}

	bool Member::HasPermission(discord::Permission perm) {
		/**
		 * @brief Check if this member has a permission
		 *
		 * ```cpp
		 *      bool has_perm = member.HasPermission(discord::Permission::ADMINISTRATOR);
		 * ```
		 *
		 * @param[in] perm The permission to check that the member has.
		 *
		 * @return bool
		 */

		discord::Permissions permissions;

		if (roles.size() > 0) {
			for (discord::Role role : roles) {
				if (roles.front() == role) {
					permissions.allow_perms.value = role.permissions.allow_perms.value;
					permissions.deny_perms.value = role.permissions.deny_perms.value;
				} else {
					permissions.allow_perms.value |= role.permissions.allow_perms.value;
					permissions.deny_perms.value |= role.permissions.deny_perms.value;
				}
			}
		}

		this->permissions = permissions;

		return permissions.allow_perms.HasPermission(perm) && !permissions.deny_perms.HasPermission(perm);
	}
}