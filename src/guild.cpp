#include <memory>
#include "guild.h"
#include "client.h"

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

		auto it = discpp::globals::client_instance->guilds.find(id);

		if (it != discpp::globals::client_instance->guilds.end()) {
			*this = *it->second;
		}
	}

	Guild::Guild(rapidjson::Document& json) {
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

		id = json["id"].GetString();
        name = json["name"].GetString();
		icon = GetDataSafely<std::string>(json, "name");
		splash = GetDataSafely<std::string>(json, "splash");
		discovery_splash = GetDataSafely<std::string>(json, "discovery_splash");
		owner = GetDataSafely<bool>(json, "owner");
		owner_id = GetDataSafely<std::string>(json, "owner");
		permissions = GetDataSafely<int>(json, "permissions");
		region = json["region"].GetString();
		afk_channel_id = GetDataSafely<snowflake>(json, "afk_channel_id");
		afk_timeout = json["afk_timeout"].GetInt();
		embed_enabled = GetDataSafely<bool>(json, "embed_enabled");
		embed_channel_id = GetDataSafely<snowflake>(json, "embed_channel_id");
		verification_level = static_cast<discpp::specials::VerificationLevel>(json["verification_level"].GetInt());
		default_message_notifications = static_cast<discpp::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].GetInt());
		explicit_content_filter = static_cast<discpp::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].GetInt());

		if (ContainsNotNull(json, "roles")) {
			for (auto const& role : json["roles"].GetArray()) {
                rapidjson::Document role_json;
                role_json.CopyFrom(role, role_json.GetAllocator());

				discpp::Role tmp = discpp::Role(role_json);
				roles.insert({ tmp.id, std::make_shared<discpp::Role>(tmp) });
			}
		}

        if (ContainsNotNull(json, "emojis")) {
            for (auto const& emoji : json["emojis"].GetArray()) {
                rapidjson::Document emoji_json;
                emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

                discpp::Emoji tmp = discpp::Emoji(emoji_json);
                emojis.insert({ tmp.id, std::make_shared<discpp::Emoji>(tmp) });
            }
        }

        if (ContainsNotNull(json, "features")) {
            for (auto const& feature : json["features"].GetArray()) {
                rapidjson::Document feature_json;
                feature_json.CopyFrom(feature, feature_json.GetAllocator());

                features.push_back(feature_json.GetString());
            }
        }

		mfa_level = static_cast<discpp::specials::MFALevel>(json["mfa_level"].GetInt());
		application_id = GetDataSafely<std::string>(json, "application_id");
		widget_enabled = GetDataSafely<bool>(json, "widget_enabled");
		widget_channel_id = GetDataSafely<std::string>(json, "widget_channel_id");
		system_channel_id = GetDataSafely<std::string>(json, "system_channel_id");
		system_channel_flags = json["system_channel_flags"].GetInt();
        rules_channel_id = GetDataSafely<std::string>(json, "rules_channel_id");
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		large = GetDataSafely<bool>(json, "large");
		unavailable = GetDataSafely<bool>(json, "unavailable");
		member_count = GetDataSafely<int>(json, "member_count");

        if (ContainsNotNull(json, "voice_states")) {
            for (auto const& voice_state : json["voice_states"].GetArray()) {
                rapidjson::Document voice_state_json;
                voice_state_json.CopyFrom(voice_state, voice_state_json.GetAllocator());

                discpp::VoiceState tmp(voice_state_json);
                voice_states.push_back(tmp);
            }
        }

        if (ContainsNotNull(json, "channels")) {
            for (auto const& channel : json["channels"].GetArray()) {
                rapidjson::Document channel_json;
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::GuildChannel tmp = discpp::GuildChannel(channel_json);
                channels.insert({ tmp.id, std::make_shared<discpp::GuildChannel>(tmp)});
            }
        }

		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");
		vanity_url_code = GetDataSafely<std::string>(json, "vanity_url_code");
		description = GetDataSafely<std::string>(json, "description");
		banner = GetDataSafely<std::string>(json, "banner");
		premium_tier = static_cast<discpp::specials::NitroTier>(json["premium_tier"].GetInt());
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = json["preferred_locale"].GetString();

		if (ContainsNotNull(json, "public_updates_channel_id")) {
            discpp::Channel channel = ConstructDiscppObjectFromID(json, "public_updates_channel_id", discpp::Channel());
            public_updates_channel = std::make_shared<discpp::GuildChannel>(discpp::GuildChannel(channel.id, this->id));
        }

		created_at = FormatTimeFromSnowflake(id);

        if (ContainsNotNull(json, "members")) {
            for (auto const& member : json["members"].GetArray()) {
                rapidjson::Document member_json;
                member_json.CopyFrom(member, member_json.GetAllocator());

                discpp::Member tmp = discpp::Member(member_json, *this);
                members.insert({ tmp.id, std::make_shared<discpp::Member>(tmp)});
            }
        }

		if (ContainsNotNull(json, "presences") && ContainsNotNull(json, "members")) {
            for (auto const& presence : json["presences"].GetArray()) {
                rapidjson::Document presence_json;
                presence_json.CopyFrom(presence, presence_json.GetAllocator());

                std::unordered_map<snowflake, std::shared_ptr<Member>>::iterator it = members.find(presence_json["user"]["id"].GetString());

                if (it != members.end()) {
                    rapidjson::Document activity_json;
                    if (ContainsNotNull(json, "game")) {
                        activity_json.CopyFrom(json["game"], activity_json.GetAllocator());
                    }

                    discpp::Activity act;

                    if (ContainsNotNull(presence_json, "status")) {
                        act.status = presence_json["status"].GetString();
                    }

                    if (!activity_json.IsNull()) {
                        act.text = activity_json["name"].GetString();
                        act.type = static_cast<presence::ActivityType>(activity_json["type"].GetInt());
                        if (ContainsNotNull(activity_json, "url")) {
                            act.url = activity_json["url"].GetString();
                        }
                        act.application_id = activity_json["id"].GetString();
                        act.created_at = std::to_string(activity_json["created_at"].GetInt());
                    }

                    it->second->activity = act;
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

		if (discpp::globals::client_instance->client_user.id != this->owner_id) {
			throw new NotGuildOwnerException();
		}

		SendDeleteRequest(Endpoint("/guilds/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

    std::unordered_map<discpp::snowflake, std::shared_ptr<discpp::GuildChannel>> Guild::GetChannels() {
		/**
		 * @brief Gets a list of channels in this guild.
		 *
		 * The first element in the map is the id of the channel, while the second in the channel.
		 * This makes it easy to find a channel in the array by using the `std::unordered_map::find()` method.
		 *
		 * ```cpp
		 *      std::unordered_map<discpp::snowflake, discpp::GuildChannel> guild.GetChannels();
		 * ```
		 *
		 * @return std::unordered_map<discpp::snowflake, discpp::GuildChannel>
		 */

		rapidjson::Document json = SendGetRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
        std::unordered_map<discpp::snowflake, std::shared_ptr<discpp::GuildChannel>> channels;
        for (auto const& channel : json.GetArray()) {
            if (!channel.IsNull()) {
                rapidjson::Document channel_json;
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                std::shared_ptr<discpp::GuildChannel> guild_channel = std::make_shared<discpp::GuildChannel>(discpp::GuildChannel(channel_json));
                channels.insert({ guild_channel->id, guild_channel });
            }
        }

        this->channels = channels;
		return channels;
	}

    std::shared_ptr<discpp::GuildChannel> Guild::GetChannel(snowflake id) {
	    auto it = channels.find(id);
	    if (it != channels.end()) {
	        return it->second;
	    }

		return nullptr;
	}

    std::shared_ptr<discpp::GuildChannel> Guild::CreateChannel(std::string name, std::string topic, ChannelType type, int bitrate, int user_limit, int rate_limit_per_user, int position, std::vector<discpp::Permissions> permission_overwrites, discpp::Channel category, bool nsfw) {
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
		 * @return std::shared_ptr<discpp::GuildChannel>
		 */

		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

		if (bitrate < 8000) bitrate = 8000;

		rapidjson::Document channel_json(rapidjson::kObjectType);
        rapidjson::Document::AllocatorType& allocator = channel_json.GetAllocator();

		rapidjson::Value permission_json_array(rapidjson::kArrayType);

		for (auto permission : permission_overwrites) {
			permission_json_array.PushBack(permission.ToJson(), allocator);
		}

		channel_json.AddMember("name", name, allocator);
        channel_json.AddMember("type", type, allocator);
        channel_json.AddMember("rate_limit_per_user", rate_limit_per_user, allocator);
        channel_json.AddMember("position", position, allocator);
        channel_json.AddMember("nsfw", nsfw, allocator);

		if (!topic.empty()) channel_json.AddMember("topic", EscapeString(topic), allocator);
		if (type == ChannelType::GUILD_VOICE) {
            channel_json.AddMember("bitrate", bitrate, allocator);
            channel_json.AddMember("user_limit", user_limit, allocator);
		}

		if (permission_json_array.Size() > 0) {
            channel_json.AddMember("permission_overwrites", permission_json_array, allocator);
		}

		if (!category.id.empty()) {
            channel_json.AddMember("parent_id", category.id, allocator);
		}


		cpr::Body body(DumpJson(channel_json));
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);

		discpp::GuildChannel channel(result);
        channels.insert({ channel.id, std::make_shared<discpp::GuildChannel>(channel)});

		return std::make_shared<discpp::GuildChannel>(channel);
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
		SendPatchRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Member> Guild::GetMember(snowflake id) {
		/**
		 * @brief Gets a discpp::Member from this guild.
		 *
		 * ```cpp
		 *      discpp::Member member = guild.GetMember("228846961774559232");
		 * ```
		 *
		 * @param[in] id The member's id
		 *
		 * @return std::shared_ptr<discpp::Member>
		 */

		auto it = members.find(id);

        if (it != members.end()) {
            return it->second;
        }

		globals::client_instance->logger->Error(LogTextColor::RED + "Member not found (Exceptions like these should be handled)!");
		throw std::runtime_error("Member not found (Exceptions like these should be handled)!");
	}

	void Guild::EnsureBotPermission(Permission req_perm) {
		/**
		 * @brief Ensures the bot has a permission.
		 *
		 * If the bot does not have the permission a discpp::NoPermissionException will be thrown.
		 *
		 * ```cpp
		 *      Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);
		 * ```
		 *
		 * @param[in] req_perm The permission to check if the bot has.
		 *
		 */

		std::shared_ptr<Member> tmp = this->GetMember(discpp::globals::client_instance->client_user.id);
		if (this->owner_id != tmp->id && !tmp->HasPermission(req_perm) && !tmp->HasPermission(Permission::ADMINISTRATOR)) {
			globals::client_instance->logger->Error(LogTextColor::RED + "The bot does not have permission: " + PermissionToString(req_perm) + " (Exceptions like these should be handled)!");

			throw NoPermissionException(req_perm);
		}
	}

	std::shared_ptr<discpp::Member> Guild::AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discpp::Role>& roles, bool mute, bool deaf) {
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
		 * @return std::shared_ptr<discpp::Member>
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
		rapidjson::Document result = SendPutRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::make_shared<discpp::Member>((result.Empty()) ? discpp::Member(id, *this) : discpp::Member(result, *this)); // If the member is already added, return it.
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

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		
		std::vector<discpp::GuildBan> guild_bans;
        for (auto const& guild_ban : result.GetArray()) {
            if (!guild_ban.IsNull()) {
                rapidjson::Document guild_ban_json;
                guild_ban_json.CopyFrom(guild_ban, guild_ban_json.GetAllocator());

                std::string reason = GetDataSafely<std::string>(guild_ban_json, "reason");
                discpp::User user = ConstructDiscppObjectFromJson(guild_ban_json, "user", discpp::User());
                guild_bans.push_back(discpp::GuildBan(reason, user));
            }
        }

		return guild_bans;
	}

	std::string Guild::GetMemberBanReason(discpp::Member& member) {
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

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (ContainsNotNull(result, "reason")) return result["reason"].GetString();

		return "";
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
		SendPutRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
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

	std::shared_ptr<discpp::Role> Guild::GetRole(snowflake id) {
		/**
		 * @brief Retrieve a guild role.
		 *
		 * ```cpp
		 *      discpp::Role new_role = guild.GetRole(638157816325996565)
		 * ```
		 *
		 * @param[in] id The id of the role you want to retrieve
		 *
		 * @return std::shared_ptr<discpp::Role>
		 */

		auto it = roles.find(id);
		if (it != roles.end()) {
			return it->second;
		}

		throw std::runtime_error("Role not found!");
	}

    std::shared_ptr<discpp::Role> Guild::CreateRole(std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
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
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> new_role = std::make_shared<discpp::Role>(discpp::Role(result));

		roles.insert({ new_role->id, new_role });

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
		SendPatchRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Role> Guild::ModifyRole(discpp::Role role, std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
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
		rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> modified_role = std::make_shared<discpp::Role>(discpp::Role(result));

		auto it = roles.find(role.id);
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
		SendDeleteRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);

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

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);

		return GetDataSafely<int>(result, "pruned");
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
		SendPostRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
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

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/invites"), DefaultHeaders(), {}, {});

		std::vector<discpp::GuildInvite> guild_invites;
        for (auto const& guild_invite : result.GetArray()) {
            if (!guild_invite.IsNull()) {
                rapidjson::Document guild_invite_json;
                guild_invite_json.CopyFrom(guild_invite, guild_invite_json.GetAllocator());

                guild_invites.push_back(discpp::GuildInvite(guild_invite_json));
            }
        }

		return guild_invites;
	}

	std::vector<discpp::Integration> Guild::GetIntegrations() {
		/**
		 * @brief Get guild integrations.
		 *
		 * ```cpp
		 *      std::vector<discpp::Integration> integration = guild.GetIntegrations();
		 * ```
		 *
		 * @return std::vector<discpp::Integration>
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/integrations"), DefaultHeaders(), {}, {});

		std::vector<discpp::Integration> guild_integrations;
		for (auto const& guild_integration : result.GetArray()) {
            if (!guild_integration.IsNull()) {
                rapidjson::Document guild_integration_json;
                guild_integration_json.CopyFrom(guild_integration, guild_integration_json.GetAllocator());

                guild_integrations.push_back(discpp::Integration(guild_integration_json));
            }
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

	void Guild::ModifyIntegration(discpp::Integration& guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons) {
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

	void Guild::DeleteIntegration(discpp::Integration& guild_integration) {
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

	void Guild::SyncIntegration(discpp::Integration& guild_integration) {
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

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
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
        rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discpp::GuildEmbed(result);
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
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/widget.png"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return result.GetString();
	}

	std::unordered_map<snowflake, std::shared_ptr<Emoji>> Guild::GetEmojis() {
		/**
		 * @brief Get all guild emojis.
		 *
		 * ```cpp
		 *      std::unordered_map<snowflake, std::shared_ptr<Emoji>> guild_emojis = guild.GetEmojis();
		 * ```
		 *
		 * @return std::unordered_map<snowflake, std::shared_ptr<Emoji>>
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders(), {}, {});

		std::unordered_map<snowflake, std::shared_ptr<Emoji>> emojis;
        for (auto const& emoji : result.GetArray()) {
            if (!emoji.IsNull()) {
                rapidjson::Document emoji_json;
                emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

                std::shared_ptr<discpp::Emoji> tmp = std::make_shared<discpp::Emoji>(discpp::Emoji(emoji_json));
                emojis.insert({ tmp->id, tmp });
            }
        }

        this->emojis = emojis;
		return emojis;
	}

    std::shared_ptr<Emoji> Guild::GetEmoji(snowflake id) {
		/**
		 * @brief Get a guild emoji.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji = guild.GetEmoji(685895680115605543);
		 * ```
		 *
		 * @param[in] id The emoji's id.
		 *
		 * @return std::shared_ptr<Emoji>
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), {}, {});

		return std::make_shared<discpp::Emoji>(discpp::Emoji(result));
	}

    std::shared_ptr<Emoji> Guild::CreateEmoji(std::string name, discpp::Image image, std::vector<discpp::Role>& roles) {
		/**
		 * @brief Create a guild emoji.
		 *
		 * ```cpp
		 *      ctx.guild->CreateEmoji("test", { &std::ifstream("C:\\emoji.png", std::ios::in | std::ios::binary), "C:\\emoji.png" }, {});
		 * ```
		 *
		 * @param[in] name The emoji name.
		 * @param[in] image The image for the emoji.
		 * @param[in] roles The roles for the emoji.
		 *
		 * @return std::shared_ptr<Emoji>
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
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

        std::shared_ptr<Emoji> emoji = std::make_shared<Emoji>(discpp::Emoji(result));
        emojis.insert({ emoji->id, emoji });

		return emoji;
	}

    std::shared_ptr<Emoji> Guild::ModifyEmoji(discpp::Emoji& emoji, std::string name, std::vector<discpp::Role>& roles) {
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
		 * @return std::shared_ptr<Emoji>
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
		rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		std::shared_ptr<discpp::Emoji> resulted_emoji = std::make_shared<discpp::Emoji>(discpp::Emoji(result));

		auto it = emojis.find(resulted_emoji->id);
		if (it != emojis.end()) {
			it->second = resulted_emoji;
		}

		return resulted_emoji;
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
		SendDeleteRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);

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

	std::shared_ptr<discpp::Member> Guild::GetOwnerMember() {
		/**
		 * @brief Retrieve guild owner as a discpp::Member object
		 *
		 * ```cpp
		 *      std::shared_ptr<discpp::Member> owner = guild.GetOwnerObject()
		 * ```
		 *
		 * @return std::shared_ptr<discpp::Member>
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
        rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + id), headers, id, RateLimitBucketType::CHANNEL, body);

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
         *      discpp::GuildInvite vanity_url = ctx.guild->GetVanityURL();
         * ```
         *
         * @return discpp::GuildInvite
         */

	    rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + id + "/vanity-url"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::GuildInvite(result);
    }
}