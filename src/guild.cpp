#include <memory>
#include "guild.h"
#include "client.h"

namespace discpp {
	Guild::Guild(const snowflake& id) : DiscordObject(id) {
		auto it = discpp::globals::client_instance->guilds.find(id);

		if (it != discpp::globals::client_instance->guilds.end()) {
			*this = *it->second;
		}
	}

	Guild::Guild(rapidjson::Document& json) {
		id = SnowflakeFromString(json["id"].GetString());
        name = json["name"].GetString();
		icon = GetDataSafely<std::string>(json, "icon");
		splash = GetDataSafely<std::string>(json, "splash");
		discovery_splash = GetDataSafely<std::string>(json, "discovery_splash");
		if (GetDataSafely<bool>(json, "owner")) flags |= 0b1;
		owner_id = GetIDSafely(json, "owner_id");
		permissions = GetDataSafely<int>(json, "permissions");
		region = json["region"].GetString();
		afk_channel_id = GetIDSafely(json, "afk_channel_id");
		afk_timeout = json["afk_timeout"].GetInt();
        if (GetDataSafely<bool>(json, "embed_enabled")) flags |= 0b10;
		embed_channel_id = GetIDSafely(json, "embed_channel_id");
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
                emojis.insert({ tmp.id, tmp });
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
		application_id = GetIDSafely(json, "application_id");
        if (GetDataSafely<bool>(json, "widget_enabled")) flags |= 0b100;
		widget_channel_id = GetIDSafely(json, "widget_channel_id");
		system_channel_id = GetIDSafely(json, "system_channel_id");
		system_channel_flags = json["system_channel_flags"].GetInt();
        rules_channel_id = GetIDSafely(json, "rules_channel_id");
		joined_at = GetDataSafely<std::string>(json, "joined_at");
        if (GetDataSafely<bool>(json, "large")) flags |= 0b1000;
        if (GetDataSafely<bool>(json, "unavailable")) flags |= 0b10000;
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
                channels.insert({ tmp.id, tmp });
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
            public_updates_channel = std::make_shared<discpp::GuildChannel>(discpp::GuildChannel(SnowflakeFromString(json["public_updates_channel_id"].GetString()), this->id));
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

                std::unordered_map<snowflake, std::shared_ptr<Member>>::iterator it = members.find(SnowflakeFromString(presence_json["user"]["id"].GetString()));

                if (it != members.end()) {
                    rapidjson::Document activity_json;
                    if (ContainsNotNull(json, "game")) {
                        activity_json.CopyFrom(json["game"], activity_json.GetAllocator());
                    }

                    discpp::Presence presence(presence_json);

                    it->second->presence = presence;
                }
            }
		}
	}

	void Guild::DeleteGuild() {
		if (discpp::globals::client_instance->client_user.id != this->owner_id) {
			throw new NotGuildOwnerException();
		}

		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

    std::unordered_map<discpp::snowflake, discpp::GuildChannel> Guild::GetChannels() {
		rapidjson::Document json = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
        std::unordered_map<discpp::snowflake, discpp::GuildChannel> channels;
        for (auto const& channel : json.GetArray()) {
            if (!channel.IsNull()) {
                rapidjson::Document channel_json;
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::GuildChannel guild_channel = discpp::GuildChannel(channel_json);
                channels.insert({ guild_channel.id, guild_channel });
            }
        }

        this->channels = channels;
		return channels;
	}

    std::unordered_map<discpp::snowflake, discpp::CategoryChannel> Guild::GetCategories() {
	    std::unordered_map<discpp::snowflake, discpp::CategoryChannel> tmp;
	    for (auto& chnl : this->GetChannels()) {
	        if (chnl.second.type == discpp::ChannelType::GROUP_CATEGORY) {
	            tmp.insert({chnl.first, CategoryChannel(chnl.first, chnl.second.guild_id)});
	        } else {
	            continue;
	        }
	    }
	    return tmp;
	}

    discpp::GuildChannel Guild::GetChannel(const snowflake& id) const {
	    auto it = channels.find(id);
	    if (it != channels.end()) {
	        return it->second;
	    }

		return discpp::GuildChannel();
	}

    discpp::GuildChannel Guild::CreateChannel(const std::string& name, const std::string& topic, const ChannelType& type, const int& bitrate, const int& user_limit, const int& rate_limit_per_user, const int& position, const std::vector<discpp::Permissions>& permission_overwrites, const discpp::CategoryChannel& category, const bool& nsfw) {
		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);
        int tmp = bitrate;
		if (tmp < 8000) tmp = 8000;

		rapidjson::Document channel_json(rapidjson::kObjectType);
        rapidjson::Document::AllocatorType& allocator = channel_json.GetAllocator();

		rapidjson::Value permission_json_array(rapidjson::kArrayType);

		for (auto permission : permission_overwrites) {
			permission_json_array.PushBack(std::move(permission.ToJson()), allocator);
		}

		channel_json.AddMember("name", name, allocator);
        channel_json.AddMember("type", type, allocator);
        channel_json.AddMember("rate_limit_per_user", rate_limit_per_user, allocator);
        channel_json.AddMember("position", position, allocator);
        channel_json.AddMember("nsfw", nsfw, allocator);

		if (!topic.empty()) channel_json.AddMember("topic", EscapeString(topic), allocator);
		if (type == ChannelType::GUILD_VOICE) {
            channel_json.AddMember("bitrate", tmp, allocator);
            channel_json.AddMember("user_limit", user_limit, allocator);
		}

		if (permission_json_array.Size() > 0) {
            channel_json.AddMember("permission_overwrites", permission_json_array, allocator);
		}

		if (category.id != 0) {
            channel_json.AddMember("parent_id", category.id, allocator);
		}


		cpr::Body body(DumpJson(channel_json));
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);

		discpp::GuildChannel channel(result);
        channels.insert({ channel.id, channel });

		return channel;
	}

	void Guild::ModifyChannelPositions(const std::vector<discpp::Channel>& new_channel_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

		rapidjson::Document json_raw(rapidjson::kArrayType);
		auto array = json_raw.GetArray();

		for (int i = 0; i < new_channel_positions.size(); i++) {
		    rapidjson::Document channel_pos_json(rapidjson::kObjectType);
		    channel_pos_json.AddMember("id", new_channel_positions[i].id, channel_pos_json.GetAllocator());
            channel_pos_json.AddMember("position", i, channel_pos_json.GetAllocator());

			json_raw.PushBack(channel_pos_json, json_raw.GetAllocator());
		}

		cpr::Body body(DumpJson(json_raw));
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Member> Guild::GetMember(const snowflake& id) const {
		auto it = members.find(id);

        if (it != members.end()) {
            return it->second;
        }

		throw std::runtime_error("Member not found (Exceptions like these should be handled)!");
	}

	void Guild::EnsureBotPermission(const Permission& req_perm) const {
		std::shared_ptr<Member> tmp = this->GetMember(discpp::globals::client_instance->client_user.id);
		if (this->owner_id != tmp->id && !tmp->HasPermission(req_perm) && !tmp->HasPermission(Permission::ADMINISTRATOR)) {
			globals::client_instance->logger->Error(LogTextColor::RED + "The bot does not have permission: " + PermissionToString(req_perm) + " (Exceptions like these should be handled)!");

			throw NoPermissionException(req_perm);
		}
	}

	std::shared_ptr<discpp::Member> Guild::AddMember(const snowflake& id, const std::string& access_token, const std::string& nick, const std::vector<discpp::Role>& roles, const bool& mute, const bool& deaf) {
		std::string json_roles = "[";
		for (discpp::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + std::to_string(role.id) + "\"";
			} else {
				json_roles += ", \"" + std::to_string(role.id) + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");
		rapidjson::Document result = SendPutRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::make_shared<discpp::Member>((result.Empty()) ? discpp::Member(id, *this) : discpp::Member(result, *this)); // If the member is already added, return it.
	}

	void Guild::RemoveMember(const discpp::Member& member) {
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member.user.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discpp::GuildBan> Guild::GetBans() const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		
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

	std::string Guild::GetMemberBanReason(const discpp::Member& member) const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(member.user.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (ContainsNotNull(result, "reason")) return result["reason"].GetString();

		return "";
	}

	void Guild::BanMember(const discpp::Member& member, const std::string& reason) {
		BanMemberById(member.user.id, reason);
	}

    void Guild::BanMemberById(const discpp::snowflake& user_id, const std::string& reason) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
        cpr::Body body("{\"reason\": \"" + EscapeString(reason) + "\"}");
        SendPutRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
    }

	void Guild::UnbanMember(const discpp::Member& member) {
		UnbanMemberById(member.user.id);
	}

    void Guild::UnbanMemberById(const snowflake& user_id) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
        SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
    }

	void Guild::KickMember(const discpp::Member& member) {
		KickMemberById(member.user.id);
	}

    void Guild::KickMemberById(const snowflake& member_id) {
        Guild::EnsureBotPermission(Permission::KICK_MEMBERS);
        SendDeleteRequest(Endpoint("guilds/" + std::to_string(id) + "/members/" + std::to_string(member_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::shared_ptr<discpp::Role> Guild::GetRole(const snowflake& id) const {
		auto it = roles.find(id);
		if (it != roles.end()) {
			return it->second;
		}

		throw std::runtime_error("Role not found!");
	}

    std::shared_ptr<discpp::Role> Guild::CreateRole(const std::string& name, const Permissions& permissions, const int& color, const bool& hoist, const bool& mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

		rapidjson::Document json_body(rapidjson::kObjectType);
		json_body.AddMember("name", EscapeString(name), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

		cpr::Body body(DumpJson(json_body));
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> new_role = std::make_shared<discpp::Role>(discpp::Role(result));

		roles.insert({ new_role->id, new_role });

		return new_role;
	}

	void Guild::ModifyRolePositions(const std::vector<discpp::Role>& new_role_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

        rapidjson::Document json_raw(rapidjson::kArrayType);
		for (int i = 0; i < new_role_positions.size(); i++) {
            rapidjson::Document role_pos_json(rapidjson::kObjectType);
            role_pos_json.AddMember("id", new_role_positions[i].id, role_pos_json.GetAllocator());
            role_pos_json.AddMember("position", i, role_pos_json.GetAllocator());

            json_raw.PushBack(role_pos_json, json_raw.GetAllocator());
        }

		cpr::Body body(DumpJson(json_raw));
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Role> Guild::ModifyRole(const discpp::Role& role, const std::string& name, const Permissions& permissions, const int& color, const bool& hoist, const bool& mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

		rapidjson::Document json_body(rapidjson::kObjectType);
        json_body.AddMember("name", EscapeString(name), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

		cpr::Body body(DumpJson(json_body));
		rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> modified_role = std::make_shared<discpp::Role>(discpp::Role(result));

		auto it = roles.find(role.id);
		if (it != roles.end()) {
			it->second = modified_role;
		}

		return modified_role;
	}

	void Guild::DeleteRole(const discpp::Role& role) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);

		roles.erase(role.id);
	}

	int Guild::GetPruneAmount(const int& days) const {
		if (days < 1) {
			throw std::runtime_error("Cannot get prune amount with less than 1 day.");
		}

		cpr::Body body("{\"days\": " + std::to_string(days) + "}");

		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);

		return GetDataSafely<int>(result, "pruned");
	}

	void Guild::BeginPrune(const int& days) {
		cpr::Body body("{\"days\": " + std::to_string(days) + "}");
		SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
	}

	std::vector<discpp::GuildInvite> Guild::GetInvites() const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});

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

	std::vector<discpp::Integration> Guild::GetIntegrations() const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/integrations"), DefaultHeaders(), {}, {});

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

	void Guild::CreateIntegration(const snowflake& id, const std::string& type) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		cpr::Body body("{\"type\": \"" + type + "\", \"id\": \"" + std::to_string(id) + "\"}");
		SendPostRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/integrations"), DefaultHeaders(), this->id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(const discpp::Integration& guild_integration, const int& expire_behavior, const int& expire_grace_period, const bool& enable_emoticons) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		cpr::Body body("{\"expire_behavior\": " + std::to_string(expire_behavior) + ", \"expire_grace_period\": " + std::to_string(expire_grace_period) + ", \"enable_emoticons\": " + std::to_string(enable_emoticons) + "}");
		SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::DeleteIntegration(const discpp::Integration& guild_integration) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::SyncIntegration(const discpp::Integration& guild_integration) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id) + "/sync"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discpp::GuildEmbed Guild::GetGuildEmbed() const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return discpp::GuildEmbed(result);
	}

	discpp::GuildEmbed Guild::ModifyGuildEmbed(const snowflake& channel_id, const bool& enabled) {
		cpr::Body body("{\"channel_id\": \"" + std::to_string(channel_id) + "\", \"enabled\": " + ((enabled) ? "true" : "false") + "}");
        rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discpp::GuildEmbed(result);
	}

	std::string Guild::GetWidgetImageURL(const WidgetStyle& widget_style) const {

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
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/widget.png"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return result.GetString();
	}

	std::unordered_map<snowflake, Emoji> Guild::GetEmojis() {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders(), {}, {});

		std::unordered_map<snowflake, Emoji> emojis;
        for (auto const& emoji : result.GetArray()) {
            if (!emoji.IsNull()) {
                rapidjson::Document emoji_json;
                emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

                discpp::Emoji tmp = discpp::Emoji(emoji_json);
                emojis.insert({ tmp.id, tmp });
            }
        }

        this->emojis = emojis;
		return emojis;
	}

    Emoji Guild::GetEmoji(const snowflake& id) const {
		rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), {}, {});

		return discpp::Emoji(result);
	}

    Emoji Guild::CreateEmoji(const std::string& name, discpp::Image& image, const std::vector<discpp::Role>& roles) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);

		rapidjson::Document role_json(rapidjson::kObjectType);
		for (discpp::Role role : roles) {
			role_json.PushBack(role.id, role_json.GetAllocator());
		}

        rapidjson::Document body_raw(rapidjson::kObjectType);
        body_raw.AddMember("name", EscapeString(name), body_raw.GetAllocator());
        body_raw.AddMember("image", image.ToDataURI(), body_raw.GetAllocator());
        body_raw.AddMember("roles", role_json, body_raw.GetAllocator());

		cpr::Body body(DumpJson(body_raw));
		rapidjson::Document result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

        Emoji emoji = discpp::Emoji(result);
        emojis.insert({ emoji.id, emoji });

		return emoji;
	}

    Emoji Guild::ModifyEmoji(const discpp::Emoji& emoji, const std::string& name, const std::vector<discpp::Role>& roles) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
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

		cpr::Body body("{\"name\": \"" + name + "\", \"roles\": " + json_roles + "}");
		rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		discpp::Emoji resulted_emoji = discpp::Emoji(result);

		auto it = emojis.find(resulted_emoji.id);
		if (it != emojis.end()) {
			it->second = resulted_emoji;
		}

		return resulted_emoji;
	}

	void Guild::DeleteEmoji(const discpp::Emoji& emoji) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);

		emojis.erase(emoji.id);
	}

	std::string Guild::GetIconURL(const discpp::ImageType& imgType) const {
		discpp::ImageType tmp;
		std::string url = "https://cdn.discordapp.com/icons/" + std::to_string(id) +  "/" + this->icon;
		tmp = imgType;
		if (tmp == ImageType::AUTO) tmp = StartsWith(this->icon, "a_") ? ImageType::GIF : ImageType::PNG;
		switch (tmp) {
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

	inline std::shared_ptr<discpp::Member> Guild::GetOwnerMember() const {
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
            default:
                return "";
        }
    }

    // Helper type for the visitor
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

    discpp::Guild Guild::Modify(GuildModifyRequests modify_requests) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
        cpr::Header headers = DefaultHeaders({ {"Content-Type", "application/json" } });
        std::string field;

        rapidjson::Document j_body(rapidjson::kObjectType);
        for (auto request : modify_requests.guild_requests) {
            std::variant<std::string, int, Image> variant = request.second;
            std::visit(overloaded {
                    [&](int i) { j_body[GuildPropertyToString(request.first)].SetInt(i); },
                    [&](Image img) { j_body[GuildPropertyToString(request.first)].SetString(rapidjson::StringRef(img.ToDataURI())); },
                    [&](const std::string& str) { j_body[GuildPropertyToString(request.first)].SetString(rapidjson::StringRef(str)); }
            }, variant);
        }

        cpr::Body body(DumpJson(j_body));
        rapidjson::Document result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id)), headers, id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Guild(result);
        return *this;
    }

    discpp::GuildInvite Guild::GetVanityURL() const {

	    rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/vanity-url"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::GuildInvite(result);
    }

    discpp::AuditLog Guild::GetAuditLog() const {
        rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/audit-logs"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::AuditLog(result);
    }

    bool Guild::IsBotOwner() const {
        return (flags & 0b1) == 0b1;
    }

    bool Guild::IsEmbedEnabled() const {
        return (flags & 0b10) == 0b10;
    }

    bool Guild::IsWidgetEnabled() const {
        return (flags & 0b100) == 0b100;
    }

    bool Guild::IsLarge() const {
        return (flags & 0b1000) == 0b1000;
    }

    bool Guild::IsUnavailable() const {
        return (flags & 0b10000) == 0b10000;
    }

    std::shared_ptr<discpp::Member> Guild::RequestMemberIfNotExist(const discpp::snowflake& member_id) {
        auto it = members.find(member_id);
        if (it != members.end()) {
            return it->second;
        } else {
            rapidjson::Document result = discpp::SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);

            std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(result, *this);
            members.insert({ member_id, member });

            return member;
        }
    }
}