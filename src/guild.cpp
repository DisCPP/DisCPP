#include "guild.h"
#include "bot.h"

namespace discpp {
	Guild::Guild(snowflake id) : DiscordObject(id) {
		/**
		 * @brief Constructs a discpp::Guild object from an id.
		 *
		 * This constructor searches the guild cache to get a guild object.
		 *
		 * ```cpp
		 *      discpp::Guild guild(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the guild
		 *
		 * @return discpp::Guild, this is a constructor.
		 */

		std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::bot_instance->guilds.find(id);
		if (it != discpp::globals::bot_instance->guilds.end()) {
			*this = it->second;
		}
	}

	Guild::Guild(nlohmann::json json) {
		/**
		 * @brief Constructs a discpp::Guild object by parsing json
		 *
		 * ```cpp
		 *      discpp::Guild guild(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the guild.
		 *
		 * @return discpp::Guild, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		icon = GetDataSafely<std::string>(json, "icon");
		name = GetDataSafely<std::string>(json, "name");
		splash = GetDataSafely<std::string>(json, "splash");
		owner = GetDataSafely<bool>(json, owner);
		owner_id = GetDataSafely<snowflake>(json, "owner_id");
		permissions = GetDataSafely<int>(json, "permissions");
		region = GetDataSafely<std::string>(json, "region");
		afk_channel_id = GetDataSafely<snowflake>(json, "afk_channel_id");
		afk_timeout = GetDataSafely<int>(json, "afk_timeout");
		embed_enabled = GetDataSafely<bool>(json, "embed_enabled");
		embed_channel_id = GetDataSafely<snowflake>(json, "embed_channel_id");
		verification_level = (json.contains("verification_level")) ? static_cast<discpp::specials::VerificationLevel>(json["verification_level"].get<int>()) : discpp::specials::VerificationLevel::NO_VERIFICATION;
		default_message_notifications = (json.contains("default_message_notifications")) ? static_cast<discpp::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].get<int>()) : discpp::specials::DefaultMessageNotificationLevel::ALL_MESSAGES;
		explicit_content_filter = (json.contains("explicit_content_filter")) ? static_cast<discpp::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].get<int>()) : discpp::specials::ExplicitContentFilterLevel::DISABLED;
		if (json.contains("roles")) {
			for (auto& role : json["roles"]) {
				discpp::Role tmp = discpp::Role(role);
				roles.insert(std::make_pair<snowflake, Role>(static_cast<discpp::snowflake>(tmp.id), static_cast<discpp::Role>(tmp)));
			}
		}
		if (json.contains("emojis")) {
			for (auto& emoji : json["emojis"]) {
				discpp::Emoji tmp = discpp::Emoji(emoji);
				emojis.insert(std::make_pair<snowflake, Emoji>(static_cast<discpp::snowflake>(tmp.id), static_cast<discpp::Emoji>(tmp)));
			}
		}
		for (auto const& feature : json["features"]) {
		    features.push_back(feature);
		}
		mfa_level = (json.contains("mfa_level")) ? static_cast<discpp::specials::MFALevel>(json["mfa_level"].get<int>()) : discpp::specials::MFALevel::NO_MFA;
		application_id = GetDataSafely<snowflake>(json, "application_id");
		widget_enabled = GetDataSafely<bool>(json, "widget_enabled");
		widget_channel_id = GetDataSafely<snowflake>(json, "widget_channel_id");
		system_channel_id = GetDataSafely<snowflake>(json, "system_channel_id");
		system_channel_flags = GetDataSafely<int>(json, "system_channel_flags");
        rules_channel_id = GetDataSafely<snowflake>(json, "rules_channel_id");
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		large = GetDataSafely<bool>(json, "large");
		unavailable = GetDataSafely<bool>(json, "unavailable");
		member_count = GetDataSafely<int>(json, "member_count");
		for (auto const& voice_state : json["voice_states"]) {
		    voice_states.push_back({voice_state});
		}
		if (json.contains("channels")) {
			for (auto& channel : json["channels"]) {
				discpp::GuildChannel tmp = discpp::GuildChannel(channel);
				channels.insert(std::make_pair<snowflake, GuildChannel>(static_cast<discpp::snowflake>(tmp.id), static_cast<discpp::GuildChannel>(tmp)));
			}
		}
		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");
		vanity_url_code = GetDataSafely<std::string>(json, "vanity_url_code");
		description = GetDataSafely<std::string>(json, "description");
		banner = GetDataSafely<std::string>(json, "banner");
		premium_tier = (json.contains("premium_tier")) ? static_cast<discpp::specials::NitroTier>(json["premium_tier"].get<int>()) : discpp::specials::NitroTier::NO_TIER;
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = GetDataSafely<std::string>(json, "preferred_locale");
		created_at = FormatTimeFromSnowflake(id);
		if (json.contains("members")) {
			for (auto& member : json["members"]) {
				discpp::Member tmp = discpp::Member(member, *this);
				members.insert(std::make_pair<snowflake, Member>(static_cast<discpp::snowflake>(tmp.id), static_cast<discpp::Member>(tmp)));
			}
		}
		if (json.contains("presences") && json.contains("members")) {
			for (auto const& presence : json["presences"]) {
				std::unordered_map<snowflake, Member>::iterator it = members.find(presence["user"]["id"]);

				if (it != members.end()) {
					nlohmann::json activity = presence["game"];

					discpp::Activity act;

					if (!presence["status"].is_null()) {
						act.status = presence["status"];
					}

					if (!activity.is_null()) {
						act.text = activity["name"];
						act.type = static_cast<presence::ActivityType>(activity["type"].get<int>());
						if (activity.contains("url")) {
							act.url = activity["url"];
						}
						act.application_id = activity["id"];
						act.created_at = std::to_string(activity["created_at"].get<int>());
					}

					it->second.activity = act;
				}
			}
		}
	}

	void Guild::DeleteGuild() {
		/**
		 * @brief Deletes this guild.
		 *
		 * ```cpp
		 *      guild.DeleteGuild();
		 * ```
		 *
		 * @return void
		 */
		if (discpp::globals::bot_instance->bot_user.id != this->owner_id) {
			throw new NotGuildOwner();
		}
		SendDeleteRequest(Endpoint("/guilds/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discpp::GuildChannel> Guild::GetChannels() {
		/**
		 * @brief Gets a list of channels in this guild.
		 *
		 * ```cpp
		 *      std::vector<discpp::Channel> guild.GetChannels();
		 * ```
		 *
		 * @return std::vector<discpp::Channel>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		std::vector<discpp::GuildChannel> channels;
		for (auto& channel : result) {
			channels.push_back(discpp::GuildChannel(channel));
		}
		return channels;
	}

	discpp::GuildChannel Guild::GetChannel(snowflake id) {
		return this->channels.find(id)->second;
	}

	discpp::GuildChannel Guild::CreateChannel(std::string name, std::string topic, ChannelType type, int bitrate, int user_limit, int rate_limit_per_user, int position, std::vector<discpp::Permissions> permission_overwrites, discpp::Channel category, bool nsfw) {
		/**
		 * @brief Creates a channel for this Guild.
		 *
		 * ```cpp
		 *      discpp::Channel channel = guild.CreateChannel("Test", discpp::GuildChannelType::GUILD_TEXT, "Just a test channel", 0, 0, 0, 0, overwrites, category_channel, false);
		 * ```
		 *
		 * @param[in] name The name of the new channel.
		 * @param[in] topic The topic of the new channel.
		 * @param[in] type The type of the new channel.
		 * @param[in] bitrate The bitrate of the new channel (only for voice channels).
		 * @param[in] user_limit The user limit of the new channel (only for voice channels).
		 * @param[in] rate_limit_per_user The chat delay for a user of the new channel.
		 * @param[in] position The sorting position of the new channel.
		 * @param[in] permission_overwrites Array of permission overwrite objects.
		 * @param[in] parent_id The parent id of the new channel.
		 * @param[in] nsfw Whether the new channel is marked as nsfw.
		 *
		 * @return discpp::Channel
		 */

		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

		if (bitrate < 8000) bitrate = 8000;

		nlohmann::json permission_json = nlohmann::json::array();
		for (auto perm : permission_overwrites) {
			permission_json.push_back(perm.ToJson());
		}

		nlohmann::json json_raw = nlohmann::json({
			{"name", name},
			{"type", type},
			{"rate_limit_per_user", rate_limit_per_user},
			{"position", position},
			{"nsfw", nsfw}
		});

		if (!topic.empty()) json_raw.push_back({ "topic", EscapeString(topic) });
		if (type == ChannelType::GUILD_VOICE) {
			json_raw.push_back({ "bitrate", bitrate });
			json_raw.push_back({ "user_limit", user_limit });
		}

		if (permission_json.size() > 0) {
			json_raw.push_back({ "permission_overwrites", permission_json });
		}

		if (!category.id.empty()) {
			json_raw.push_back({ "parent_id", category.id });
		}


		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
		discpp::GuildChannel channel(result);
		channels.insert(std::pair<snowflake, GuildChannel>(static_cast<snowflake>(channel.id), static_cast<GuildChannel>(channel)));

		return channel;
	}

	void Guild::ModifyChannelPositions(std::vector<discpp::Channel>& new_channel_positions) {
		/**
		 * @brief Modifies channel's positions in order to vector elements.
		 *
		 * ```cpp
		 *      guild.ModifyChannelPositions(new_channel_positions);
		 * ```
		 *
		 * @param[in] new_channel_positions The new channel positions in order of the vector elements.
		 *
		 * @return void
		 */

		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

		nlohmann::json json_raw = nlohmann::json();

		for (int i = 0; i < new_channel_positions.size(); i++) {
			json_raw.push_back({ {"id", new_channel_positions[i].id}, {"position", i} });
		}

		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	discpp::Member Guild::GetMember(snowflake id) {
		/**
		 * @brief Gets a discpp::Member from this guild.
		 *
		 * ```cpp
		 *      discpp::Member member = guild.GetMember(228846961774559232);
		 * ```
		 *
		 * @param[in] id The member's id
		 *
		 * @return discpp::Member
		 */

		std::unordered_map<snowflake, Member>::iterator it = discpp::globals::bot_instance->members.find(id);
		if (it != discpp::globals::bot_instance->members.end()) {
			return it->second;
		}
		//throw std::runtime_error("Member not found!");
		return discpp::Member();
	}

	void Guild::EnsureBotPermission(Permission reqPerm) {
		/**
		 * @brief Ensures the bot has permission
		 *
		 * ```cpp
		 *      Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);
		 * ```
		 *
		 * @param[in] reqPerm the permission in question
		 *
		 */
		Member tmp = this->GetMember(discpp::globals::bot_instance->bot_user.id);
		if ((this->owner_id != tmp.id) || (!tmp.HasPermission(reqPerm)) || (!tmp.HasPermission(Permission::ADMINISTRATOR))) {
			throw new NoPermissionException(reqPerm);
		}
	}

	discpp::Member Guild::AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discpp::Role>& roles, bool mute, bool deaf) {
		/**
		 * @brief Adds a discpp::Member to this guild.
		 *
		 * ```cpp
		 *      discpp::Member added_member =  guild.AddMember(119886831578775554, access_token, "New User is here", roles, false, false);
		 * ```
		 *
		 * @param[in] id The id of this member.
		 * @param[in] access_token The oauth2 token granted with guilds.join to the bot's application for the member.
		 * @param[in] nick The user nick name.
		 * @param[in] roles The users role.
		 * @param[in] mute Whether the user is muted in voice channels.
		 * @param[in] deaf Whether the user is deafened in voice channels.
		 *
		 * @return discpp::Member
		 */

		std::string json_roles = "[";
		for (discpp::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			} else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");
		nlohmann::json result = SendPutRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		return (result == "{}") ? discpp::Member(id) : discpp::Member(result, id); // If the member is already added, return it.
	}

	void Guild::RemoveMember(discpp::Member& member) {
		/**
		 * @brief Remove a member from the guild.
		 *
		 * ```cpp
		 *      guild.RemoveMember(member);
		 * ```
		 *
		 * @param[in] member The member to remove from the guild.
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/guilds/" + id + "/members/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discpp::GuildBan> Guild::GetBans() {
		/**
		 * @brief Get all guild bans
		 *
		 * ```cpp
		 *      std::vector<discpp::GuildBan> bans = guild.GetBans();
		 * ```
		 *
		 * @return std::vector<discpp::GuildBan>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		
		std::vector<discpp::GuildBan> guild_bans;
		for (auto& guild_ban : result) {
			std::string reason = (!guild_ban["reason"].is_null()) ? guild_ban["reason"] : "";
			guild_bans.push_back(discpp::GuildBan(reason, discpp::User(guild_ban["user"])));
		}

		return guild_bans;
	}

	std::optional<std::string> Guild::GetMemberBanReason(discpp::Member& member) {
		/**
		 * @brief Get ban reasons if they are any.
		 *
		 * ```cpp
		 *      std::optional<std::string> reason = guild.GetMemberBanReason(member);
		 * ```
		 *
		 * @param[in] member The member to get a ban remove of.
		 *
		 * @return std::optional<std::string>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (result.contains("reason")) return result["reason"];

		return std::nullopt;
	}

	void Guild::BanMember(discpp::Member& member, std::string reason) {
		/**
		 * @brief Ban a guild member.
		 *
		 * ```cpp
		 *      guild.BanMember(member, "Reason");
		 * ```
		 *
		 * @param[in] member The member to ban.
		 * @param[in] reason The reason to ban them.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
		cpr::Body body("{\"reason\": \"" + EscapeString(reason) + "\"}");
		nlohmann::json json = SendPutRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::UnbanMember(discpp::Member& member) {
		/**
		 * @brief Unban a guild member.
		 *
		 * ```cpp
		 *      std::optional<std::string> reason = guild.GetMemberBanReason(member);
		 * ```
		 *
		 * @param[in] member The member to unban.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
		SendDeleteRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::KickMember(discpp::Member& member) {
		/**
		 * @brief Kick a guild member.
		 *
		 * ```cpp
		 *      guild.KickMember(member);
		 * ```
		 *
		 * @param[in] member The member to ban.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::KICK_MEMBERS);
		SendDeleteRequest(Endpoint("guilds/" + id + "/members/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discpp::Role Guild::GetRole(snowflake id) {
		/**
		 * @brief Retrieve a guild role.
		 *
		 * ```cpp
		 *      discpp::Role new_role = guild.GetRole(638157816325996565)
		 * ```
		 *
		 * @param[in] id The id of the role you want to retrieve
		 *
		 * @return discpp::Role
		 */

		std::unordered_map<snowflake, Role>::iterator it = roles.find(id);
		if (it != roles.end()) {
			return it->second;
		}
		throw std::runtime_error("Role not found!");
	}

	discpp::Role Guild::CreateRole(std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		/**
		 * @brief Create a guild role.
		 *
		 * ```cpp
		 *      discpp::Role new_role = guild.CreateRole("New Role", permissions, 0xffffff, false, true);
		 * ```
		 *
		 * @param[in] name The new role name.
		 * @param[in] permissions The new role permissions.
		 * @param[in] color The new role color.
		 * @param[in] hoist Whether or not to hoist the role.
		 * @param[in] mentionable Whether or not the role is mentionable.
		 *
		 * @return discpp::Role
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
		nlohmann::json json_body = nlohmann::json({
			{"name", EscapeString(name)},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discpp::Role new_role(result);
		roles.insert(std::pair<snowflake, Role>(new_role.id, new_role));

		return new_role;
	}

	void Guild::ModifyRolePositions(std::vector<discpp::Role>& new_role_positions) {
		/**
		 * @brief Modifies role's positions in order to vector elements.
		 *
		 * ```cpp
		 *      guild.ModifyRolePositions(new_role_positions);
		 * ```
		 *
		 * @param[in] new_role_positions The new role positions in order of the vector elements.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
		nlohmann::json json_raw = nlohmann::json();

		for (int i = 0; i < new_role_positions.size(); i++) {
			json_raw.push_back({ {"id", new_role_positions[i].id}, {"position", i} });
		}

		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	discpp::Role Guild::ModifyRole(discpp::Role role, std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		/**
		 * @brief Create a guild role.
		 *
		 * ```cpp
		 *      discpp::Role modified_role = guild.ModifyRole(role, "New Role", permissions, 0xffffff, false, true);
		 * ```
		 *
		 * @param[in] name The new role name.
		 * @param[in] permissions The new role permissions.
		 * @param[in] color The new role color.
		 * @param[in] hoist Whether or not to hoist the role.
		 * @param[in] mentionable Whether or not the role is mentionable.
		 *
		 * @return discpp::Role
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
		nlohmann::json json_body = nlohmann::json({
			{"name", name},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discpp::Role modified_role(result);
		std::unordered_map<snowflake, Role>::iterator it = roles.find(role.id);
		if (it != roles.end()) {
			it->second = modified_role;
		}

		return modified_role;
	}

	void Guild::DeleteRole(discpp::Role& role) {
		/**
		 * @brief Deleted a guild role.
		 *
		 * ```cpp
		 *      guild.DeleteRole(role);
		 * ```
		 *
		 * @param[in] role The role to delete.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		roles.erase(role.id);
	}

	int Guild::GetPruneAmount(int days) {
		/**
		 * @brief Get guild prune amount.
		 *
		 * ```cpp
		 *      int amount_pruned = guild.GetPruneAmount(10);
		 * ```
		 *
		 * @param[in] days Number of days to count prune for.
		 *
		 * @return int - Amount of users pruned.
		 */

		if (days < 1) {
			throw std::runtime_error("Cannot get prune amount with less than 1 day.");
		}

		cpr::Body body("{\"days\": " + std::to_string(days) + "}");

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
		
		return result["pruned"].get<int>();
	}

	void Guild::BeginPrune(int days) {
		/**
		 * @brief Begin a guild prune (kicks inactive players).
		 *
		 * ```cpp
		 *      guild.BeginPrune(14);
		 * ```
		 *
		 * @param[in] days Number of days to prune.
		 *
		 * @return void
		 */

		cpr::Body body("{\"days\": " + std::to_string(days) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
	}

	std::vector<discpp::GuildInvite> Guild::GetInvites() {
		/**
		 * @brief Get guild invites.
		 *
		 * ```cpp
		 *      std::vector<discpp::GuildInvite> invites = guild.GetInvites();
		 * ```
		 *
		 * @return std::vector<discpp::GuildInvite>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/invites"), DefaultHeaders(), {}, {});

		std::vector<discpp::GuildInvite> guild_invites;
		for (auto& guild_invite : result) {
			guild_invites.push_back(discpp::GuildInvite(guild_invite));
		}

		return guild_invites;
	}

	std::vector<discpp::GuildIntegration> Guild::GetIntegrations() {
		/**
		 * @brief Get guild integrations.
		 *
		 * ```cpp
		 *      std::vector<discpp::GuildIntegration> integration = guild.GetIntegrations();
		 * ```
		 *
		 * @return std::vector<discpp::GuildIntegration>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/integrations"), DefaultHeaders(), {}, {});

		std::vector<discpp::GuildIntegration> guild_integrations;
		for (auto& guild_integration : result) {
			guild_integrations.push_back(discpp::GuildIntegration(guild_integration));
		}

		return guild_integrations;
	}

	void Guild::CreateIntegration(snowflake id, std::string type) {
		/**
		 * @brief Create a guild integration.
		 *
		 * ```cpp
		 *      guild.CreateIntegration(integration_id, integration_type);
		 * ```
		 *
		 * @param[in] id The integration id.
		 * @param[in] type The integration type.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		cpr::Body body("{\"type\": \"" + type + "\", \"id\": \"" + id + "\"}");
		SendPostRequest(Endpoint("/guilds/" + this->id + "/integrations"), DefaultHeaders(), this->id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(discpp::GuildIntegration& guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons) {
		/**
		 * @brief Modify a guild integration.
		 *
		 * ```cpp
		 *      guild.ModifyIntegration(integration, 0, 2, true);
		 * ```
		 *
		 * @param[in] guild_integration The guild integration to modify.
		 * @param[in] expire_behavior The behavior when an integration subscription lapses.
		 * @param[in] expire_grace_period Period (in days) where the integration will ignore lapsed subscriptions.
		 * @param[in] enable_emoticons Whether emoticons should be synced for this integration (twitch only currently).
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		cpr::Body body("{\"expire_behavior\": " + std::to_string(expire_behavior) + ", \"expire_grace_period\": " + std::to_string(expire_grace_period) + ", \"enable_emoticons\": " + std::to_string(enable_emoticons) + "}");
		SendPostRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::DeleteIntegration(discpp::GuildIntegration& guild_integration) {
		/**
		 * @brief Delete a guild integration.
		 *
		 * ```cpp
		 *      guild.DeleteIntegration(integration);
		 * ```
		 *
		 * @param[in] guild_integration The guild integration to delete.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		SendDeleteRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::SyncIntegration(discpp::GuildIntegration& guild_integration) {
		/**
		 * @brief Sync a guild integration.
		 *
		 * ```cpp
		 *      guild.SyncIntegration(integration);
		 * ```
		 *
		 * @param[in] guild_integration The guild integration to sync.
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		SendPostRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id + "/sync"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discpp::GuildEmbed Guild::GetGuildEmbed() {
		/**
		 * @brief Get a guild embed.
		 *
		 * ```cpp
		 *      discpp::GuildEmbed guild_embed = guild.GetGuildEmbed();
		 * ```
		 *
		 * @return discpp::GuildEmbed
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return discpp::GuildEmbed(result);
	}

	discpp::GuildEmbed Guild::ModifyGuildEmbed(snowflake channel_id, bool enabled) {
		/**
		 * @brief Modify a guild embed.
		 *
		 * ```cpp
		 *      discpp::GuildEmbed modified_guild_embed = guild.ModifyGuildEmbed(381871767846780928, true);
		 * ```
		 *
		 * @return discpp::GuildEmbed
		 */

		cpr::Body body("{\"channel_id\": \"" + channel_id + "\", \"enabled\": " + ((enabled) ? "true" : "false") + "}");
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discpp::GuildEmbed();
	}

	std::string Guild::GetWidgetImageURL(WidgetStyle widget_style) {
		/**
		 * @brief Get a widget image url.
		 *
		 * ```cpp
		 *      std::string widget_image_url = guild.GetWidgetImageURL(style);
		 * ```
		 *
		 * @return std::string
		 */

		std::string style;
		switch (widget_style) {
		case WidgetStyle::SHIELD:
			style = "shield";
			break;
		case WidgetStyle::BANNER1:
			style = "banner1";
			break;
		case WidgetStyle::BANNER2:
			style = "banner2";
			break;
		case WidgetStyle::BANNER3:
			style = "banner3";
			break;
		case WidgetStyle::BANNER4:
			style = "banner4";
			break;
		}
		cpr::Body body("{\"style\": " + style + "}");
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/widget.png"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::string();
	}

	std::unordered_map<snowflake, Emoji> Guild::GetEmojis() {
		/**
		 * @brief Get all guild emojis.
		 *
		 * ```cpp
		 *      std::vector<discpp::Emoji> guild_emojis = guild.GetEmojis();
		 * ```
		 *
		 * @return std::vector<discpp::Emoji>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders(), {}, {});

		std::unordered_map<snowflake, Emoji> emojis;
		for (auto& emoji : result) {
			discpp::Emoji tmp = discpp::Emoji(emoji);
			emojis.insert(std::pair<snowflake, Emoji>(static_cast<snowflake>(tmp.id), static_cast<Emoji>(tmp)));
		}
		this->emojis = emojis;

		return emojis;
	}

	discpp::Emoji Guild::GetEmoji(snowflake id) {
		/**
		 * @brief Get a guild emoji.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji = guild.GetEmoji(685895680115605543);
		 * ```
		 *
		 * @param[in] id The emoji's id.
		 *
		 * @return discpp::Emoji
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), {}, {});

		return discpp::Emoji(result);
	}

	discpp::Emoji Guild::CreateEmoji(std::string name, discpp::Image image, std::vector<discpp::Role>& roles) {
		/**
		 * @brief Create a guild emoji.
		 *
		 * ```cpp
		 *      ctx.guild.CreateEmoji("test", { &std::ifstream("C:\\emoji.png", std::ios::in | std::ios::binary), "C:\\emoji.png" }, {});
		 * ```
		 *
		 * @param[in] name The emoji name.
		 * @param[in] image The image for the emoji.
		 * @param[in] roles The roles for the emoji.
		 *
		 * @return discpp::Emoji
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
		nlohmann::json role_json;
		for (discpp::Role role : roles) {
			role_json.push_back(role.id);
		}

		nlohmann::json body_raw = nlohmann::json({
			{"name", EscapeString(name)},
			{"image", image.ToDataURI()},
			{"roles", role_json}
		});

		cpr::Body body(body_raw.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

		return discpp::Emoji(result);
	}

	discpp::Emoji Guild::ModifyEmoji(discpp::Emoji& emoji, std::string name, std::vector<discpp::Role>& roles) {
		/**
		 * @brief Modify a guild emoji.
		 *
		 * ```cpp
		 *      discpp::Emoji modified_emoji = guild.ModifyEmoji(emoji, "New emoji", roles);
		 * ```
		 *
		 * @param[in] emoji The emoji to modify.
		 * @param[in] name The emoji's name.
		 * @param[in] role The emoji's roles.
		 *
		 * @return discpp::Emoji
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
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

		cpr::Body body("{\"name\": \"" + name + "\", \"roles\": " + json_roles + "}");
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discpp::Emoji em(result);
		std::unordered_map<snowflake, Emoji>::iterator it = emojis.find(em.id);
		if (it != emojis.end()) {
			it->second = em;
		}
		return em;
	}

	void Guild::DeleteEmoji(discpp::Emoji& emoji) {
		/**
		 * @brief Delete a guild emoji.
		 *
		 * ```cpp
		 *      guild.DeleteEmoji(emoji);
		 * ```
		 *
		 * @return void
		 */
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		emojis.erase(emoji.id);
	}

	std::string Guild::GetIconURL(discpp::ImageType imgType) {
		/**
		 * @brief Retrieve guild icon url.
		 *
		 * ```cpp
		 *      std::string icon_url = guild.GetIconURL()
		 * ```
		 *
		 * @param[in] imgType Optional parameter for type of image
		 *
		 * @return std::string
		 */

		std::string idString = this->id.c_str();
		std::string url = "https://cdn.discppapp.com/icons/" + idString +  "/" + this->icon;
		if (imgType == ImageType::AUTO) imgType = StartsWith(this->icon, "a_") ? ImageType::GIF : ImageType::PNG;
		switch (imgType) {
		case ImageType::GIF:
			return cpr::Url(url + ".gif");
		case ImageType::JPEG:
			return cpr::Url(url + ".jpeg");
		case ImageType::PNG:
			return cpr::Url(url + ".png");
		case ImageType::WEBP:
			return cpr::Url(url + ".webp");
		default:
			return cpr::Url(url);
		}
	}

	discpp::Member Guild::GetOwnerObject() {
		/**
		 * @brief Retrieve guild owner as a discpp::Member object
		 *
		 * ```cpp
		 *      discpp::Member owner = guild.GetOwnerObject()
		 * ```
		 *
		 * @return discpp::Member
		 */

		return this->GetMember(this->owner_id);
	}

    std::string GuildPropertyToString(GuildProperty prop) {
        switch (prop) {
            case GuildProperty::NAME:
                return "name";
            case GuildProperty::REGION:
                return "region";
            case GuildProperty::VERIFICATION_LEVEL:
                return "verification_level";
            case GuildProperty::DEFAULT_MESSAGE_NOTIFICATIONS:
                return "default_message_notifications";
            case GuildProperty::EXPLICIT_CONTENT_FILTER:
                return "explicit_content_filter";
            case GuildProperty::AFK_CHANNEL_ID:
                return "afk_channel_id";
            case GuildProperty::AFK_TIMEOUT:
                return "afk_timeout";
            case GuildProperty::ICON:
                return "icon";
            case GuildProperty::OWNER_ID:
                return "owner_id";
            case GuildProperty::SPLASH:
                return "splash";
            case GuildProperty::BANNER:
                return "banner";
            case GuildProperty::SYSTEM_CHANNEL_ID:
                return "system_channel_id";
            case GuildProperty::RULES_CHANNEL_ID:
                return "rules_channel_id";
            case GuildProperty::PUBLIC_UPDATES_CHANNEL_ID:
                return "public_updates_channel_id";
            case GuildProperty::PREFERRED_LOCALE:
                return "preferred_locale";
        }
    }

    // Helper type for the visitor
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

    discpp::Guild Guild::Modify(GuildModifyRequests modify_requests) {
        /**
         * @brief Modify the guild.
         *
         * Use discpp::GuildModifyRequests to modify a field of the guild.
         *
         * ```cpp
         *		// Change the name of the guild to "Test"
         *		discpp::ModifyRequests request(discpp::GuildProperty::NAME, "Test");
         *      guild.Modify(request);
         * ```
         *
         * @param[in] modify_request The field to modify, and what to set it to.
         *
         * @return discpp::Channel - This method also sets the guild reference to the returned guild.
         */
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
        cpr::Header headers = DefaultHeaders({ {"Content-Type", "application/json" } });
        std::string field;
        nlohmann::json j_body = {};
        for (auto request : modify_requests.guild_requests) {
            std::variant<std::string, int, Image> variant = request.second;
            std::visit(overloaded {
                    [&](int i) { j_body[GuildPropertyToString(request.first)] = i; },
                    [&](Image img) { j_body[GuildPropertyToString(request.first)] = img.ToDataURI(); },
                    [&](const std::string& str) { j_body[GuildPropertyToString(request.first)] = str; }
            }, variant);
        }

        cpr::Body body(j_body.dump());
        nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), headers, id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Guild(result);
        return *this;
    }

    discpp::GuildInvite Guild::GetVanityURL() {
        /**
         * @brief Returns a partial invite object for guilds with that feature enabled.
         *
         * Requires the MANAGE_GUILD permission. code will be null if a vanity url for the guild is not set. Only `code` and `uses` are valid
         *
         * ```cpp
         *      discpp::GuildInvite vanity_url = ctx.guild.GetVanityURL();
         * ```
         *
         * @return discpp::GuildInvite
         */

	    nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/vanity-url"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::GuildInvite(result);
    }
}