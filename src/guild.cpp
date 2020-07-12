#include "exceptions.h"
#include "guild.h"
#include "client.h"
#include "log.h"
#include "member.h"
#include "role.h"
#include "channel.h"
#include "audit_log.h"
#include "user.h"

#include <memory>

#ifdef RAPIDJSON_BACKEND
#include <rapidjson/writer.h>
#elif SIMDJSON_BACKEND

#endif

namespace discpp {
	Guild::Guild(const Snowflake& id, bool can_request) : DiscordObject(id) {
        *this = *globals::client_instance->cache.GetGuild(id, can_request);
	}

	Guild::Guild(const discpp::JsonObject& json) {
		id = SnowflakeFromString(json["id"].GetString());
        name = json["name"].GetString();

        if (json.ContainsNotNull("icon")) {
            std::string icon_str = json["icon"].GetString();

            if (StartsWith(icon_str, "a_")) {
                is_icon_gif = true;
                SplitAvatarHash(icon_str.substr(2), icon_hex);
            } else {
                SplitAvatarHash(icon_str, icon_hex);
            }
        }

        if (json.ContainsNotNull("splash")) {
            SplitAvatarHash(json["splash"].GetString(), splash_hex);
        }

        if (json.ContainsNotNull("discovery_splash")) {
            SplitAvatarHash(json["discovery_splash"].GetString(), discovery_hex);
        }

		if (json.Get<bool>("owner")) flags |= 0b1;
		owner_id = json.GetIDSafely("owner_id");
		permissions = json.Get<int>("permissions");
		region = json["region"].GetString();
		afk_channel_id = json.GetIDSafely("afk_channel_id");
		afk_timeout = json["afk_timeout"].GetInt();
        if (json.Get<bool>("embed_enabled")) flags |= 0b10;
		verification_level = static_cast<discpp::specials::VerificationLevel>(json["verification_level"].GetInt());
		default_message_notifications = static_cast<discpp::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].GetInt());
		explicit_content_filter = static_cast<discpp::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].GetInt());

		if (json.ContainsNotNull("roles")) {
            json["roles"].IterateThrough([&] (const discpp::JsonObject& role_json)->bool {
                discpp::Role tmp = discpp::Role(role_json);
                roles.emplace(tmp.id, std::make_shared<discpp::Role>(tmp));
                return true;
            });
		}

        if (json.ContainsNotNull("emojis")) {
            json["emojis"].IterateThrough([&] (const discpp::JsonObject& emoji_json)->bool {
                discpp::Emoji tmp = discpp::Emoji(emoji_json);
                emojis.emplace(tmp.id, tmp);

                return true;
            });
        }

        if (json.ContainsNotNull("features")) {
            json["features"].IterateThrough([&] (const discpp::JsonObject& feature_json)->bool {
                features.push_back(feature_json.GetString());

                return true;
            });
        }

		mfa_level = static_cast<discpp::specials::MFALevel>(json["mfa_level"].GetInt());
		application_id = json.GetIDSafely("application_id");
        if (json.Get<bool>("widget_enabled")) flags |= 0b100;
		widget_channel_id = json.GetIDSafely("widget_channel_id");
		system_channel_id = json.GetIDSafely("system_channel_id");
		system_channel_flags = json["system_channel_flags"].GetInt();
        rules_channel_id = json.GetIDSafely("rules_channel_id");

        if (json.ContainsNotNull("joined_at")) {
            joined_at = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["joined_at"].GetString()));
        }

        if (json.Get<bool>("large")) flags |= 0b1000;
        if (json.Get<bool>("unavailable")) flags |= 0b10000;
		member_count = json.Get<int>("member_count");

        if (json.ContainsNotNull("voice_states")) {
            json["voice_states"].IterateThrough([&] (const discpp::JsonObject& voice_state_json)->bool {
                discpp::VoiceState tmp(voice_state_json);
                voice_states.push_back(tmp);
                return true;
            });
        }

        if (json.ContainsNotNull("channels")) {
            json["members"].IterateThrough([&] (const discpp::JsonObject& channel_json)->bool {
                discpp::Channel tmp(channel_json);
                tmp.guild_id = id;
                channels.insert({ tmp.id, tmp });
                return true;
            });
        }

		max_presences = json.Get<int>("max_presences");
		max_members = json.Get<int>("max_members");

        if (json.ContainsNotNull("vanity_url_code")) {
            vanity_url_code = json["vanity_url_code"].GetString();
        }

        if (json.ContainsNotNull("description")) {
            description = json["description"].GetString();
        }

        if (json.ContainsNotNull("banner")) {
            SplitAvatarHash(json["banner"].GetString(), banner_hex);
        }

		premium_tier = static_cast<discpp::specials::NitroTier>(json["premium_tier"].GetInt());
		premium_subscription_count = json.Get<int>("premium_subscription_count");
		preferred_locale = json["preferred_locale"].GetString();

		if (json.ContainsNotNull("public_updates_channel_id")) {
		    auto channel = channels.find(Snowflake(json["public_updates_channel_id"].GetString()));
		    if (channel != channels.end()) {
                public_updates_channel = channel->second;
		    }
        }

        if (json.ContainsNotNull("members")) {
            json["members"].IterateThrough([&] (const discpp::JsonObject& member_json)->bool {
                discpp::Member tmp(member_json, *this);
                members.insert({ tmp.user.id, std::make_shared<discpp::Member>(tmp)});
                return true;
            });
        }

		if (json.ContainsNotNull("presences") && json.ContainsNotNull("members")) {
            json["presences"].IterateThrough([&] (const discpp::JsonObject& presence_json)->bool {
                auto it = members.find(Snowflake(presence_json["user"]["id"].GetString()));
                if (it != members.end()) {
                    it->second->presence = std::make_unique<discpp::Presence>(presence_json);
                }

                return true;
            });
		}
	}

	void Guild::DeleteGuild() {
		if (discpp::globals::client_instance->client_user.id != this->owner_id) {
			throw NotGuildOwnerException();
		}

		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

    std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::GetChannels() {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

		std::unordered_map<discpp::Snowflake, discpp::Channel> channels;
        result->IterateThrough([&] (const discpp::JsonObject& json)->bool {
            discpp::Channel guild_channel(json);
            channels.emplace(guild_channel.id, guild_channel);
            return true;
        });

        this->channels = channels;
		return channels;
	}

    std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::GetCategories() {
	    std::unordered_map<discpp::Snowflake, discpp::Channel> tmp;

	    for (auto& chnl : this->GetChannels()) {
	        if (chnl.second.type == discpp::ChannelType::GROUP_CATEGORY) {
	            tmp.emplace(chnl.first, chnl.second);
	        } else continue;
	    }

	    return tmp;
	}

	std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::GetParentlessChannels() {
	    std::unordered_map<discpp::Snowflake, discpp::Channel> tmp;

	    for (auto& chnl : this->GetChannels()) {
	        if (chnl.second.category_id == 0) {
	            tmp.emplace(std::pair(chnl.first, chnl.second));
	        }
	    }

	    return tmp;
	}

    discpp::Channel Guild::GetChannel(const Snowflake& id) const {
	    auto it = channels.find(id);
	    if (it != channels.end()) {
	        return it->second;
	    }

		return discpp::Channel();
	}

    discpp::Channel Guild::CreateChannel(const std::string& name, const std::string& topic, const ChannelType& type, const int& bitrate, const int& user_limit, const int& rate_limit_per_user, const int& position, const std::vector<discpp::Permissions>& permission_overwrites, const discpp::Snowflake& parent_id, const bool nsfw) {
		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);
        int tmp = bitrate;
		if (tmp < 8000) tmp = 8000;

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document channel_json(rapidjson::kObjectType);
        rapidjson::Document::AllocatorType& allocator = channel_json.GetAllocator();

        rapidjson::Value permission_json_array(rapidjson::kArrayType);

        for (auto permission : permission_overwrites) {
            rapidjson::Document doc;
            doc.Parse(permission.ToJson().DumpJson());
            permission_json_array.PushBack(doc, allocator);
        }

        channel_json.AddMember("name", rapidjson::StringRef(name.c_str()), allocator);
        channel_json.AddMember("type", type, allocator);
        channel_json.AddMember("rate_limit_per_user", rate_limit_per_user, allocator);
        channel_json.AddMember("position", position, allocator);
        channel_json.AddMember("nsfw", nsfw, allocator);

        if (!topic.empty()) channel_json.AddMember("topic", rapidjson::StringRef(EscapeString(topic).c_str()), allocator);
        if (type == ChannelType::GUILD_VOICE) {
            channel_json.AddMember("bitrate", tmp, allocator);
            channel_json.AddMember("user_limit", user_limit, allocator);
        }

        if (permission_json_array.Size() > 0) {
            channel_json.AddMember("permission_overwrites", permission_json_array, allocator);
        }

        if (parent_id != 0) {
            channel_json.AddMember("parent_id", parent_id, allocator);
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        channel_json.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif

		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);

        discpp::Channel channel(*result);
        channels.insert({ channel.id, channel });

		return channel;
	}

	void Guild::ModifyChannelPositions(const std::vector<discpp::Channel>& new_channel_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document json_raw(rapidjson::kArrayType);
        auto array = json_raw.GetArray();

        for (int i = 0; i < new_channel_positions.size(); i++) {
            rapidjson::Document channel_pos_json(rapidjson::kObjectType);
            channel_pos_json.AddMember("id", new_channel_positions[i].id, channel_pos_json.GetAllocator());
            channel_pos_json.AddMember("position", i, channel_pos_json.GetAllocator());

            json_raw.PushBack(channel_pos_json, json_raw.GetAllocator());
        }
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_raw.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Member> Guild::GetMember(const Snowflake& id, bool can_request) {
		if (id == 0) {
			throw exceptions::DiscordObjectNotFound("Member id: " + std::to_string(id) + " is not valid!");
		}

		auto it = members.find(id);

        if (it != members.end()) {
            return it->second;
        }

        if (can_request) {
            std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/"+ std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

            auto member = std::make_shared<discpp::Member>(*result, this->id);
            members.emplace(member->user.id, member);
            return member;
        } else {
            throw exceptions::DiscordObjectNotFound("Member not found of id: " + std::to_string(id));
        }
	}

	void Guild::EnsureBotPermission(const Permission& req_perm) {
		std::shared_ptr<Member> tmp = GetMember(discpp::globals::client_instance->client_user.id);
		if (this->owner_id != tmp->user.id && !tmp->HasPermission(req_perm) && !tmp->HasPermission(Permission::ADMINISTRATOR)) {
			globals::client_instance->logger->Error(LogTextColor::RED + "The bot does not have permission: " + PermissionToString(req_perm) + " (Exceptions like these should be handled)!");

			throw NoPermissionException(req_perm);
		}
	}

	std::shared_ptr<discpp::Member> Guild::AddMember(const Snowflake& id, const std::string& access_token, const std::string& nick, const std::vector<discpp::Role>& roles, const bool mute, const bool deaf) {
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
		std::unique_ptr<discpp::JsonObject> result = SendPutRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::make_shared<discpp::Member>((result->IsEmpty()) ? discpp::Member(id, *this) : discpp::Member(*result, *this)); // If the member is already added, return it.
	}

	void Guild::RemoveMember(const discpp::Member& member) {
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member.user.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discpp::GuildBan> Guild::GetBans() const {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		
		std::vector<discpp::GuildBan> guild_bans;
		result->IterateThrough([&] (const discpp::JsonObject& json)->bool {
            auto reason = json.Get<std::string>("reason");

            std::shared_ptr<discpp::User> user = std::make_shared<discpp::User>(json["user"]);
            guild_bans.emplace_back(reason, user);

            return true;
		});

		return guild_bans;
	}

	std::string Guild::GetMemberBanReason(const discpp::Member& member) const {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(member.user.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (result->ContainsNotNull("reason")) return (*result)["reason"].GetString();

		return "";
	}

	void Guild::BanMember(const discpp::Member& member, const std::string& reason) {
		BanMemberById(member.user.id, reason);
	}

    void Guild::BanMemberById(const discpp::Snowflake& user_id, const std::string& reason) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
        cpr::Body body("{\"reason\": \"" + EscapeString(reason) + "\"}");
        SendPutRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
    }

	void Guild::UnbanMember(const discpp::Member& member) {
		UnbanMemberById(member.user.id);
	}

    void Guild::UnbanMemberById(const Snowflake& user_id) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);
        SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);
    }

	void Guild::KickMember(const discpp::Member& member, const std::string& reason) {
		KickMemberById(member.user.id, reason);
	}

    void Guild::KickMemberById(const Snowflake& member_id, const std::string& reason) {
        Guild::EnsureBotPermission(Permission::KICK_MEMBERS);

        std::string url = Endpoint("guilds/" + std::to_string(id) + "/members/" + std::to_string(member_id));
        if (!reason.empty()) {
            url += "?reason=" + URIEncode(reason);
        }

        SendDeleteRequest(url, DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::shared_ptr<discpp::Role> Guild::GetRole(const Snowflake& id) const {
		auto it = roles.find(id);
		if (it != roles.end()) {
			return it->second;
		}

		throw std::runtime_error("Role not found!");
	}

    std::shared_ptr<discpp::Role> Guild::CreateRole(const std::string& name, const Permissions& permissions, const int& color, const bool hoist, const bool mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document json_body(rapidjson::kObjectType);
        json_body.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_body.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> new_role = std::make_shared<discpp::Role>(discpp::Role(*result));

		roles.insert({ new_role->id, new_role });

		return new_role;
	}

	void Guild::ModifyRolePositions(const std::vector<discpp::Role>& new_role_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document json_raw(rapidjson::kArrayType);
        for (int i = 0; i < new_role_positions.size(); i++) {
            rapidjson::Document role_pos_json(rapidjson::kObjectType);
            role_pos_json.AddMember("id", new_role_positions[i].id, role_pos_json.GetAllocator());
            role_pos_json.AddMember("position", i, role_pos_json.GetAllocator());

            json_raw.PushBack(role_pos_json, json_raw.GetAllocator());
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_raw.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
		SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Role> Guild::ModifyRole(const discpp::Role& role, const std::string& name, const Permissions& permissions, const int& color, const bool hoist, const bool mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document json_body(rapidjson::kObjectType);
        json_body.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_body.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
		std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> modified_role = std::make_shared<discpp::Role>(discpp::Role(*result));

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

		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);

		return result->Get<int>("pruned");
	}

	void Guild::BeginPrune(const int& days) {
		cpr::Body body("{\"days\": " + std::to_string(days) + "}");
		SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
	}

	std::vector<discpp::GuildInvite> Guild::GetInvites() const {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});

		std::vector<discpp::GuildInvite> guild_invites;
        result->IterateThrough([&] (const discpp::JsonObject& json)->bool {
            guild_invites.emplace_back(json);
            return true;
        });

		return guild_invites;
	}

	std::vector<discpp::Integration> Guild::GetIntegrations() const {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/integrations"), DefaultHeaders(), {}, {});

		std::vector<discpp::Integration> guild_integrations;
        result->IterateThrough([&] (const discpp::JsonObject& json)->bool {
            guild_integrations.emplace_back(json);
            return true;
        });

		return guild_integrations;
	}

	void Guild::CreateIntegration(const Snowflake& id, const std::string& type) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);
		cpr::Body body("{\"type\": \"" + type + "\", \"id\": \"" + std::to_string(id) + "\"}");
		SendPostRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/integrations"), DefaultHeaders(), this->id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(const discpp::Integration& guild_integration, const int& expire_behavior, const int& expire_grace_period, const bool enable_emoticons) {
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
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return discpp::GuildEmbed(*result);
	}

	discpp::GuildEmbed Guild::ModifyGuildEmbed(const Snowflake& channel_id, const bool enabled) {
		cpr::Body body("{\"channel_id\": \"" + std::to_string(channel_id) + "\", \"enabled\": " + ((enabled) ? "true" : "false") + "}");
        std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discpp::GuildEmbed(*result);
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
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/widget.png"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return result->GetString();
	}

	std::unordered_map<Snowflake, Emoji> Guild::GetEmojis() {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders(), {}, {});

		std::unordered_map<Snowflake, Emoji> emojis;
		result->IterateThrough([&] (const discpp::JsonObject& json)->bool {
            discpp::Emoji tmp = discpp::Emoji(json);
            emojis.emplace(tmp.id, tmp);
            return true;
		});

        this->emojis = emojis;
		return emojis;
	}

    Emoji Guild::GetEmoji(const Snowflake& id) const {
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), {}, {});

		return discpp::Emoji(*result);
	}

    Emoji Guild::CreateEmoji(const std::string& name, discpp::Image& image, const std::vector<discpp::Role>& roles) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document role_json(rapidjson::kObjectType);
        for (discpp::Role role : roles) {
            role_json.PushBack(role.id, role_json.GetAllocator());
        }

        rapidjson::Document body_raw(rapidjson::kObjectType);
        body_raw.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), body_raw.GetAllocator());
        body_raw.AddMember("image", rapidjson::StringRef(image.ToDataURI().c_str()), body_raw.GetAllocator());
        body_raw.AddMember("roles", role_json, body_raw.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        body_raw.Accept(writer);
        std::string tmp = buffer.GetString();

        cpr::Body body(tmp);
#elif SIMDJSON_BACKEND

#endif
		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

        Emoji emoji = discpp::Emoji(*result);
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
		std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		discpp::Emoji resulted_emoji = discpp::Emoji(*result);

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

	std::string Guild::GetIconURL(const discpp::ImageType& img_type) const {
	    std::string icon_str = CombineAvatarHash(icon_hex);

		discpp::ImageType tmp;
		std::string url = "https://cdn.discordapp.com/icons/" + std::to_string(id) +  "/" + icon_str;
		tmp = img_type;
		if (tmp == ImageType::AUTO) tmp = is_icon_gif ? ImageType::GIF : ImageType::PNG;
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

    bool Guild::HasIcon() const {
        std::string icon_str = CombineAvatarHash(icon_hex);

        return !icon_str.empty();
	}

    bool Guild::HasBanner() const {
        std::string banner_str = CombineAvatarHash(banner_hex);

        return !banner_str.empty();
    }

    bool Guild::HasSplash() const {
        std::string splash_str = CombineAvatarHash(splash_hex);

        return !splash_str.empty();
    }

    bool Guild::HasDiscoverySplash() const {
        std::string discovery_splash_str = CombineAvatarHash(discovery_hex);

        return !discovery_splash_str.empty();
    }

	inline std::shared_ptr<discpp::Member> Guild::GetOwnerMember() {
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

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document j_body(rapidjson::kObjectType);
        for (auto request : modify_requests.guild_requests) {
            std::variant<std::string, int, Image> variant = request.second;
            std::visit(overloaded {
                    [&](int i) { j_body[GuildPropertyToString(request.first).c_str()].SetInt(i); },
                    [&](Image img) { j_body[GuildPropertyToString(request.first).c_str()].SetString(rapidjson::StringRef(img.ToDataURI().c_str())); },
                    [&](const std::string& str) { j_body[GuildPropertyToString(request.first).c_str()].SetString(rapidjson::StringRef(str.c_str())); }
            }, variant);
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        j_body.Accept(writer);
        std::string tmp = buffer.GetString();

        cpr::Body body(tmp);
#elif SIMDJSON_BACKEND

#endif
        std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id)), headers, id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Guild(*result);
        return *this;
    }

    discpp::GuildInvite Guild::GetVanityURL() const {

	    std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/vanity-url"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::GuildInvite(*result);
    }

    discpp::AuditLog Guild::GetAuditLog() const {
        std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/audit-logs"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

        return discpp::AuditLog(*result);
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

    std::shared_ptr<discpp::Member> Guild::RequestMemberIfNotExist(const discpp::Snowflake& member_id) {
        auto it = members.find(member_id);
        if (it != members.end()) {
            return it->second;
        } else {
            std::unique_ptr<discpp::JsonObject> result = discpp::SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member_id)), DefaultHeaders(), id, RateLimitBucketType::GUILD);

            std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(*result, *this);
            members.insert({ member_id, member });

            return member;
        }
    }

    std::string Guild::GetBannerURL(const ImageType &img_type) const {
        std::string banner_str = CombineAvatarHash(banner_hex);

        discpp::ImageType tmp = img_type;
        std::string url = "https://cdn.discordapp.com/banners/" + std::to_string(id) +  "/" + banner_str;

        if (tmp == ImageType::AUTO) tmp = ImageType::PNG;
        switch (tmp) {
            case ImageType::GIF:
                throw exceptions::ProhibitedEndpointException("Guild banner url's can't be a GIF.");
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

    std::string Guild::GetSplashURL(const ImageType &img_type) const {
        std::string banner_str = CombineAvatarHash(banner_hex);

        discpp::ImageType tmp = img_type;
        std::string url = "https://cdn.discordapp.com/splashes/" + std::to_string(id) +  "/" + banner_str;

        if (tmp == ImageType::AUTO) tmp = ImageType::PNG;
        switch (tmp) {
            case ImageType::GIF:
                throw exceptions::ProhibitedEndpointException("Guild splash url's can't be a GIF.");
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

    std::string Guild::GetDiscoverySplashURL(const ImageType &img_type) const {
        std::string banner_str = CombineAvatarHash(banner_hex);

        discpp::ImageType tmp = img_type;
        std::string url = "https://cdn.discordapp.com/discovery-splashes/" + std::to_string(id) +  "/" + banner_str;

        if (tmp == ImageType::AUTO) tmp = ImageType::PNG;
        switch (tmp) {
            case ImageType::GIF:
                throw exceptions::ProhibitedEndpointException("Guild discovery splash url's can't be a GIF.");
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

    std::string Guild::GetFormattedJoinedAt() const {
        return FormatTime(std::chrono::system_clock::to_time_t(joined_at));
    }

    std::chrono::system_clock::time_point Guild::GetJoinedAt() const {
        return joined_at;
    }

    std::string Guild::GetFormattedCreatedAt() const {
        return FormatTime(TimeFromSnowflake(id));
    }

    std::chrono::system_clock::time_point Guild::GetCreatedAt() const {
        return std::chrono::system_clock::from_time_t(TimeFromSnowflake(id));
    }

    GuildInvite::GuildInvite(const discpp::JsonObject& json) {
        code = json["code"].GetString();
        if (json.ContainsNotNull("guild")) {
            guild = discpp::globals::client_instance->cache.GetGuild(discpp::Snowflake(json["guild"]["id"].GetString()));
        }
        channel = discpp::Channel(guild->GetChannel(Snowflake(json["channel"]["id"].GetString())));
        if (json.ContainsNotNull("inviter")) {
            inviter = std::make_shared<discpp::User>(json["inviter"]);
        }
        if (json.ContainsNotNull("target_user")) {
            target_user = std::make_shared<discpp::User>(json["target_user"]);
        }
        target_user_type = static_cast<TargetUserType>(json.Get<int>("target_user_type"));
        approximate_presence_count = json.Get<int>("approximate_presence_count");
        approximate_member_count = json.Get<int>("approximate_member_count");
    }

    VoiceState::VoiceState(const discpp::JsonObject& json) {
		guild_id = json.GetIDSafely("guild_id");
		channel_id = json.GetIDSafely("channel_id");
		user_id = SnowflakeFromString(json["user_id"].GetString());
		if (json.ContainsNotNull("member")) {
			discpp::Guild guild(guild_id);
			member = std::make_shared<discpp::Member>(json["member"], guild);
		}
		session_id = json["session_id"].GetString();
		deaf = json["deaf"].GetBool();
		mute = json["mute"].GetBool();
		self_deaf = json["self_deaf"].GetBool();
		self_mute = json["self_mute"].GetBool();
		self_stream = json.Get<bool>("self_stream");
		suppress = json["suppress"].GetBool();
    }

    Integration::Integration(const discpp::JsonObject& json) {
        id = Snowflake(json["id"].GetString());
        name = json["name"].GetString();
        type = json["type"].GetString();
        enabled = json["enabled"].GetBool();
        syncing = json["syncing"].GetBool();
        role_id = Snowflake(json["role_id"].GetString());
        enable_emoticons = json.Get<bool>("enable_emoticons");
        expire_behavior = static_cast<IntegrationExpireBehavior>(json["expire_behavior"].GetInt());
        expire_grace_period = json["expire_grace_period"].GetInt();
        if (json.ContainsNotNull("user")) {
            user = std::make_shared<discpp::User>(json["user"]);
        }

        account = json.ConstructDiscppObjectFromJson("account", discpp::IntegrationAccount());
        synced_at = json["synced_at"].GetString();
    }
}