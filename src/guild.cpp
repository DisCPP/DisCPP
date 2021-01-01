#include "exceptions.h"
#include "guild.h"
#include "client.h"
#include "log.h"
#include "member.h"
#include "role.h"
#include "channel.h"
#include "audit_log.h"
#include "user.h"
#include "cache.h"

#include <memory>

namespace discpp {
    Guild::Guild(discpp::Client* client) : DiscordObject(client) {

    }

	Guild::Guild(discpp::Client* client, const Snowflake& id, bool can_request) : DiscordObject(client, id) {
        *this = *client->cache->GetGuild(id, can_request);
	}

	Guild::Guild(discpp::Client* client, rapidjson::Document& json) : DiscordObject(client) {
		id = Snowflake(json["id"].GetString());
        name = json["name"].GetString();

        if (ContainsNotNull(json, "icon")) {
            std::string icon_str = json["icon"].GetString();

            if (StartsWith(icon_str, "a_")) {
                is_icon_gif = true;
                SplitAvatarHash(icon_str.substr(2), icon_hex);
            } else {
                SplitAvatarHash(icon_str, icon_hex);
            }
        }

        if (ContainsNotNull(json, "splash")) {
            SplitAvatarHash(json["splash"].GetString(), splash_hex);
        }

        if (ContainsNotNull(json, "discovery_splash")) {
            SplitAvatarHash(json["discovery_splash"].GetString(), discovery_hex);
        }

		if (GetDataSafely<bool>(json, "owner")) flags |= 0b1;
		owner_id = GetIDSafely(json, "owner_id");
		permissions = GetDataSafely<int>(json, "permissions");
		region = json["region"].GetString();
		afk_channel_id = GetIDSafely(json, "afk_channel_id");
		afk_timeout = json["afk_timeout"].GetInt();
        if (GetDataSafely<bool>(json, "embed_enabled")) flags |= 0b10;
		verification_level = static_cast<discpp::specials::VerificationLevel>(json["verification_level"].GetInt());
		default_message_notifications = static_cast<discpp::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].GetInt());
		explicit_content_filter = static_cast<discpp::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].GetInt());

		if (ContainsNotNull(json, "roles")) {
			for (auto const& role : json["roles"].GetArray()) {
                rapidjson::Document role_json;
                role_json.CopyFrom(role, role_json.GetAllocator());

				discpp::Role tmp = discpp::Role(client, role_json);
				roles.insert({ tmp.id, std::make_shared<discpp::Role>(tmp) });
			}
		}

        if (ContainsNotNull(json, "emojis")) {
            for (auto const& emoji : json["emojis"].GetArray()) {
                rapidjson::Document emoji_json;
                emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

                discpp::Emoji tmp = discpp::Emoji(client, emoji_json);
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

        if (ContainsNotNull(json, "joined_at")) {
            joined_at = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["joined_at"].GetString()));
        }

        if (GetDataSafely<bool>(json, "large")) flags |= 0b1000;
        if (GetDataSafely<bool>(json, "unavailable")) flags |= 0b10000;
		member_count = GetDataSafely<int>(json, "member_count");

        if (ContainsNotNull(json, "voice_states")) {
            for (auto const& voice_state : json["voice_states"].GetArray()) {
                rapidjson::Document voice_state_json;
                voice_state_json.CopyFrom(voice_state, voice_state_json.GetAllocator());

                discpp::VoiceState tmp(client, voice_state_json);
                voice_states.push_back(tmp);
            }
        }

        if (ContainsNotNull(json, "channels")) {
            for (auto const& channel : json["channels"].GetArray()) {
                rapidjson::Document channel_json;
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::Channel tmp(client, channel_json);
                tmp.guild_id = id;
                channels.insert({ tmp.id, tmp });
            }
        }

		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");

        if (ContainsNotNull(json, "vanity_url_code")) {
            vanity_url_code = json["vanity_url_code"].GetString();
        }

        if (ContainsNotNull(json, "description")) {
            description = json["description"].GetString();
        }

        if (ContainsNotNull(json, "banner")) {
            SplitAvatarHash(json["banner"].GetString(), banner_hex);
        }

		premium_tier = static_cast<discpp::specials::NitroTier>(json["premium_tier"].GetInt());
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = json["preferred_locale"].GetString();

		if (ContainsNotNull(json, "public_updates_channel_id")) {
		    auto channel = channels.find(Snowflake(json["public_updates_channel_id"].GetString()));
		    if (channel != channels.end()) {
                public_updates_channel = channel->second;
		    }
        }

        if (ContainsNotNull(json, "members")) {
            for (auto const& member : json["members"].GetArray()) {
                rapidjson::Document member_json;
                member_json.CopyFrom(member, member_json.GetAllocator());

                discpp::Member tmp(client, member_json, this);
                members.insert({ tmp.user.id, std::make_shared<discpp::Member>(tmp)});
            }
        }

		if (ContainsNotNull(json, "presences") && ContainsNotNull(json, "members")) {
            for (auto const& presence : json["presences"].GetArray()) {
                rapidjson::Document presence_json;
                presence_json.CopyFrom(presence, presence_json.GetAllocator());

                auto it = members.find(Snowflake(presence_json["user"]["id"].GetString()));

                if (it != members.end()) {
                    rapidjson::Document activity_json;
                    if (ContainsNotNull(json, "game")) {
                        activity_json.CopyFrom(json["game"], activity_json.GetAllocator());
                    }

                    it->second->presence = std::make_unique<discpp::Presence>(client, presence_json);
                }
            }
		}
	}

    Guild::Guild(const Guild& guild) {
        this->name = guild.name;
        this->owner_id = guild.owner_id;
        this->permissions = guild.permissions;
        this->region = guild.region;
        this->afk_channel_id = guild.afk_channel_id;
        this->verification_level = guild.verification_level;
        this->default_message_notifications = guild.default_message_notifications;
        this->explicit_content_filter = guild.explicit_content_filter;
        this->features = guild.features;
        this->mfa_level = guild.mfa_level;
        this->application_id = guild.application_id;
        this->widget_enabled = guild.widget_enabled;
        this->widget_channel_id = guild.widget_channel_id;
        this->system_channel_id = guild.system_channel_id;
        this->system_channel_flags = guild.system_channel_flags;
        this->rules_channel_id = guild.rules_channel_id;
        this->joined_at = guild.joined_at;
        this->member_count = guild.member_count;
        this->max_presences = guild.max_presences;
        this->max_members = guild.max_members;
        this->vanity_url_code = guild.vanity_url_code;
        this->description = guild.description;
        this->premium_tier = guild.premium_tier;
        this->premium_subscription_count = guild.premium_subscription_count;
        this->preferred_locale = guild.preferred_locale;
        this->public_updates_channel = guild.public_updates_channel;
        this->approximate_member_count = guild.approximate_member_count;
        this->approximate_presence_count = guild.approximate_presence_count;
        this->roles = guild.roles;
        this->emojis = guild.emojis;
        this->voice_states = guild.voice_states;
        this->members = guild.members;
        this->channels = guild.channels;
        this->flags = guild.flags;

        this->icon_hex[0] = guild.icon_hex[0];
        this->icon_hex[1] = guild.icon_hex[1];

        this->splash_hex[0] = guild.splash_hex[0];
        this->splash_hex[1] = guild.splash_hex[1];

        this->discovery_hex[0] = guild.discovery_hex[0];
        this->discovery_hex[1] = guild.discovery_hex[1];

        this->banner_hex[0] = guild.banner_hex[0];
        this->banner_hex[1] = guild.banner_hex[1];

        this->is_icon_gif = guild.is_icon_gif;
    }

    Guild Guild::operator=(const Guild& guild) {
        return Guild(guild);
    }

	void Guild::DeleteGuild() {
        discpp::Client* client = GetClient();
		if (client->client_user.id != this->owner_id) {
			throw NotGuildOwnerException();
		}

		SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
	}

    std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::UpdateChannels() {
        std::lock_guard<std::mutex> lock_guard(channels_mutex);

        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
        std::unordered_map<discpp::Snowflake, discpp::Channel> channels;

        for (auto const& channel : result->GetArray()) {
            if (!channel.IsNull()) {
                rapidjson::Document channel_json;
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::Channel guild_channel(client, channel_json);
                channels.insert({ guild_channel.id, guild_channel });
            }
        }

        this->channels = channels;
		return channels;
	}

    std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::GetCategories() {
	    std::unordered_map<discpp::Snowflake, discpp::Channel> tmp;

	    for (auto& chnl : channels) {
	        if (chnl.second.type == discpp::ChannelType::GROUP_CATEGORY) {
	            tmp.emplace(chnl.first, chnl.second);
	        } else continue;
	    }

	    return tmp;
	}

	std::unordered_map<discpp::Snowflake, discpp::Channel> Guild::GetParentlessChannels() {
	    std::unordered_map<discpp::Snowflake, discpp::Channel> tmp;

	    for (auto& chnl : channels) {
	        if (chnl.second.category_id == 0) {
	            tmp.emplace(std::pair(chnl.first, chnl.second));
	        }
	    }

	    return tmp;
	}

    discpp::Channel Guild::GetChannel(const Snowflake& id) {
        std::lock_guard<std::mutex> lock_guard(channels_mutex);

	    auto it = channels.find(id);
	    if (it != channels.end()) {
	        return it->second;
	    }

		throw discpp::exceptions::DiscordObjectNotFound("Failed to find cached channel.");
	}

    discpp::Channel Guild::CreateChannel(const std::string& name, const std::string& topic, const ChannelType& type, const int& bitrate, const int& user_limit, const int& rate_limit_per_user, const int& position, const std::vector<discpp::Permissions>& permission_overwrites, const discpp::Snowflake& parent_id, const bool nsfw) {
		this->EnsureBotPermission(Permission::MANAGE_CHANNELS);
        int tmp = bitrate;
		if (tmp < 8000) tmp = 8000;

		rapidjson::Document channel_json(rapidjson::kObjectType);
        rapidjson::Document::AllocatorType& allocator = channel_json.GetAllocator();

		rapidjson::Value permission_json_array(rapidjson::kArrayType);

		for (auto permission : permission_overwrites) {
			rapidjson::Document doc = permission.ToJson();
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
            channel_json.AddMember("parent_id", (uint64_t) parent_id, allocator);
		}


        std::string body(DumpJson(channel_json));

        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);

        discpp::Channel channel(client, *result);
        channels.insert({ channel.id, channel });

		return channel;
	}

	void Guild::ModifyChannelPositions(const std::vector<discpp::Channel>& new_channel_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);

		rapidjson::Document json_raw(rapidjson::kArrayType);
		auto array = json_raw.GetArray();

		for (int i = 0; i < new_channel_positions.size(); i++) {
		    rapidjson::Document channel_pos_json(rapidjson::kObjectType);
		    channel_pos_json.AddMember("id", (uint64_t) new_channel_positions[i].id, channel_pos_json.GetAllocator());
            channel_pos_json.AddMember("position", i, channel_pos_json.GetAllocator());

			json_raw.PushBack(channel_pos_json, json_raw.GetAllocator());
		}

        std::string body(DumpJson(json_raw));
        discpp::Client* client = GetClient();
		SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Member> Guild::GetMember(const Snowflake& id, bool can_request) {
        std::lock_guard<std::mutex> lock_guard(members_mutex);

		if (id == 0) {
			throw exceptions::DiscordObjectNotFound("Member id: " + std::to_string(id) + " is not valid!");
		}

		auto it = members.find(id);
        if (it != members.end()) {
            return it->second;
        }

        if (can_request) {
            discpp::Client* client = GetClient();
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/members/"+ std::to_string(id)), DefaultHeaders(client), id, RateLimitBucketType::CHANNEL);

            auto member = std::make_shared<discpp::Member>(client, *result, this);
            members.emplace(member->user.id, member);
            return member;
        } else {
            throw exceptions::DiscordObjectNotFound("Member not found of id: " + std::to_string(id));
        }
	}

	void Guild::EnsureBotPermission(const Permission& req_perm) {
        discpp::Client* client = GetClient();
		std::shared_ptr<Member> tmp = GetMember(client->client_user.id);
		if (this->owner_id != tmp->user.id && !tmp->HasPermission(req_perm) && !tmp->HasPermission(Permission::ADMINISTRATOR)) {
			client->logger->Error(LogTextColor::RED + "The bot does not have permission: " + PermissionToString(req_perm) + " (Exceptions like these should be handled)!");

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

        std::string body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");

        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPutRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);

		return std::make_shared<discpp::Member>((result->Empty()) ? discpp::Member(client, id, this) : discpp::Member(client, *result, this)); // If the member is already added, return it.
	}

	void Guild::RemoveMember(const discpp::Member& member) {
        discpp::Client* client = GetClient();
		SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member.user.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
	}

	std::vector<discpp::GuildBan> Guild::GetBans() const {
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/bans"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
		
		std::vector<discpp::GuildBan> guild_bans;
        for (auto const& guild_ban : result->GetArray()) {
            if (!guild_ban.IsNull()) {
                rapidjson::Document guild_ban_json(rapidjson::kObjectType);
                guild_ban_json.CopyFrom(guild_ban, guild_ban_json.GetAllocator());

                std::string reason;
                if (ContainsNotNull(guild_ban_json, "reason")) {
                    reason = guild_ban_json["reason"].GetString();
                }

                rapidjson::Document user_json(rapidjson::kObjectType);
                user_json.CopyFrom(guild_ban_json["user"], user_json.GetAllocator());
                std::shared_ptr<discpp::User> user = std::make_shared<discpp::User>(client, user_json);

                guild_bans.push_back(discpp::GuildBan(reason, user));
            }
        }

		return guild_bans;
	}

	std::string Guild::GetMemberBanReason(const discpp::Member& member) const {
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(member.user.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
		if (ContainsNotNull(*result, "reason")) return (*result)["reason"].GetString();

		return "";
	}

	void Guild::BanMember(const discpp::Member& member, const std::string& reason, uint16_t delete_message_days) {
		BanMemberById(member.user.id, reason, delete_message_days);
	}

    void Guild::BanMemberById(const discpp::Snowflake& user_id, const std::string& reason, uint16_t delete_message_days) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);

        discpp::Client* client = GetClient();
        std::string body("{\"reason\": \"" + EscapeString(reason) + "\", \"delete_message_days\": " + std::to_string(delete_message_days) + "}");
        SendPutRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);
    }

	void Guild::UnbanMember(const discpp::Member& member) {
		UnbanMemberById(member.user.id);
	}

    void Guild::UnbanMemberById(const Snowflake& user_id) {
        Guild::EnsureBotPermission(Permission::BAN_MEMBERS);

        discpp::Client* client = GetClient();
        SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/bans/" + std::to_string(user_id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
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

        discpp::Client* client = GetClient();
        SendDeleteRequest(client, url, DefaultHeaders(client), id, RateLimitBucketType::GUILD);
	}

	std::shared_ptr<discpp::Role> Guild::GetRole(const Snowflake& id) {
        std::lock_guard<std::mutex> lock_guard(roles_mutex);

		auto it = roles.find(id);
		if (it != roles.end()) {
			return it->second;
		}

		throw std::runtime_error("Role not found!");
	}

    std::shared_ptr<discpp::Role> Guild::CreateRole(const std::string& name, const Permissions& permissions, const int& color, const bool hoist, const bool mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

		rapidjson::Document json_body(rapidjson::kObjectType);
		json_body.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

        std::string body(DumpJson(json_body));
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> new_role = std::make_shared<discpp::Role>(discpp::Role(client, *result));

		roles.insert({ new_role->id, new_role });

		return new_role;
	}

	void Guild::ModifyRolePositions(const std::vector<discpp::Role>& new_role_positions) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

        rapidjson::Document json_raw(rapidjson::kArrayType);
		for (int i = 0; i < new_role_positions.size(); i++) {
            rapidjson::Document role_pos_json(rapidjson::kObjectType);
            role_pos_json.AddMember("id", (uint64_t) new_role_positions[i].id, role_pos_json.GetAllocator());
            role_pos_json.AddMember("position", i, role_pos_json.GetAllocator());

            json_raw.PushBack(role_pos_json, json_raw.GetAllocator());
        }

        std::string body(DumpJson(json_raw));
        discpp::Client* client = GetClient();
		SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/roles"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::CHANNEL, body);
	}

	std::shared_ptr<discpp::Role> Guild::ModifyRole(const discpp::Role& role, const std::string& name, const Permissions& permissions, const int& color, const bool hoist, const bool mentionable) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);

		rapidjson::Document json_body(rapidjson::kObjectType);
        json_body.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), json_body.GetAllocator());
        json_body.AddMember("permissions", permissions.allow_perms.value, json_body.GetAllocator());
        json_body.AddMember("color", color, json_body.GetAllocator());
        json_body.AddMember("hoist", hoist, json_body.GetAllocator());
        json_body.AddMember("mentionable", mentionable, json_body.GetAllocator());

        std::string body(DumpJson(json_body));
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);
		std::shared_ptr<discpp::Role> modified_role = std::make_shared<discpp::Role>(discpp::Role(client, *result));

		auto it = roles.find(role.id);
		if (it != roles.end()) {
			it->second = modified_role;
		}

		return modified_role;
	}

	void Guild::DeleteRole(const discpp::Role& role) {
		Guild::EnsureBotPermission(Permission::MANAGE_ROLES);
        discpp::Client* client = GetClient();
		SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/roles/" + std::to_string(role.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);

		roles.erase(role.id);
	}

	int Guild::GetPruneAmount(const int& days) const {
		if (days < 1) {
			throw std::runtime_error("Cannot get prune amount with less than 1 day.");
		}

        std::string body("{\"days\": " + std::to_string(days) + "}");

        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);

		return GetDataSafely<int>(*result, "pruned");
	}

	void Guild::BeginPrune(const int& days) {
        std::string body("{\"days\": " + std::to_string(days) + "}");
        discpp::Client* client = GetClient();
		SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/prune"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::GUILD, body);
	}

	std::vector<discpp::GuildInvite> Guild::GetInvites() const {
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/invites"), DefaultHeaders(client), {}, {});

		std::vector<discpp::GuildInvite> guild_invites;
        for (auto const& guild_invite : result->GetArray()) {
            if (!guild_invite.IsNull()) {
                rapidjson::Document guild_invite_json;
                guild_invite_json.CopyFrom(guild_invite, guild_invite_json.GetAllocator());

                guild_invites.push_back(discpp::GuildInvite(client, guild_invite_json));
            }
        }

		return guild_invites;
	}

	std::vector<discpp::Integration> Guild::GetIntegrations() const {
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/integrations"), DefaultHeaders(client), {}, {});

		std::vector<discpp::Integration> guild_integrations;
		for (auto const& guild_integration : result->GetArray()) {
            if (!guild_integration.IsNull()) {
                rapidjson::Document guild_integration_json;
                guild_integration_json.CopyFrom(guild_integration, guild_integration_json.GetAllocator());

                guild_integrations.emplace_back(guild_integration_json);
            }
        }

		return guild_integrations;
	}

	void Guild::CreateIntegration(const Snowflake& id, const std::string& type) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);

        std::string body("{\"type\": \"" + type + "\", \"id\": \"" + std::to_string(id) + "\"}");
        discpp::Client* client = GetClient();
		SendPostRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/integrations"), DefaultHeaders(client), this->id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(const discpp::Integration& guild_integration, const int& expire_behavior, const int& expire_grace_period, const bool enable_emoticons) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);

        std::string body("{\"expire_behavior\": " + std::to_string(expire_behavior) + ", \"expire_grace_period\": " + std::to_string(expire_grace_period) + ", \"enable_emoticons\": " + std::to_string(enable_emoticons) + "}");
        discpp::Client* client = GetClient();
		SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::DeleteIntegration(const discpp::Integration& guild_integration) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);

        discpp::Client* client = GetClient();
		SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
	}

	void Guild::SyncIntegration(const discpp::Integration& guild_integration) {
		Guild::EnsureBotPermission(Permission::MANAGE_GUILD);

        discpp::Client* client = GetClient();
		SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/integrations/" + std::to_string(guild_integration.id) + "/sync"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
	}

	discpp::GuildEmbed Guild::GetGuildEmbed() const {
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);
		return discpp::GuildEmbed(*result);
	}

	discpp::GuildEmbed Guild::ModifyGuildEmbed(const Snowflake& channel_id, const bool enabled) {
        std::string body("{\"channel_id\": \"" + std::to_string(channel_id) + "\", \"enabled\": " + ((enabled) ? "true" : "false") + "}");
        discpp::Client* client = GetClient();
        std::unique_ptr<rapidjson::Document> result = SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/embed"), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);

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
        std::string body("{\"style\": " + style + "}");
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/widget.png"), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);

		return result->GetString();
	}

	std::unordered_map<Snowflake, Emoji> Guild::UpdateEmojis() {
        std::lock_guard<std::mutex> lock_guard(emojis_mutex);

        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders(client), {}, {});

		std::unordered_map<Snowflake, Emoji> emojis;
        for (auto const& emoji : result->GetArray()) {
            if (!emoji.IsNull()) {
                rapidjson::Document emoji_json;
                emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

                discpp::Emoji tmp = discpp::Emoji(client, emoji_json);
                emojis.insert({ tmp.id, tmp });
            }
        }

        this->emojis = emojis;
		return emojis;
	}

    Emoji Guild::GetEmoji(const Snowflake& id, bool can_request) {
        std::lock_guard<std::mutex> lock_guard(emojis_mutex);

        if (id == 0) {
            throw exceptions::DiscordObjectNotFound("Member id: " + std::to_string(id) + " is not valid!");
        }

        auto it = emojis.find(id);
        if (it != emojis.end()) {
            return it->second;
        }

        if (can_request) {
            discpp::Client* client = GetClient();
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(client), {}, {});

            discpp::Emoji emoji(client, *result);
            emojis.emplace(emoji.id, emoji);
            return emoji;
        } else {
            throw exceptions::DiscordObjectNotFound("Member not found of id: " + std::to_string(id));
        }
	}

    Emoji Guild::CreateEmoji(const std::string& name, discpp::Image& image, const std::vector<discpp::Role>& roles) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);

		rapidjson::Document role_json(rapidjson::kObjectType);
		for (discpp::Role role : roles) {
			role_json.PushBack((uint64_t) role.id, role_json.GetAllocator());
		}

        rapidjson::Document body_raw(rapidjson::kObjectType);
        body_raw.AddMember("name", rapidjson::StringRef(EscapeString(name).c_str()), body_raw.GetAllocator());
        body_raw.AddMember("image", rapidjson::StringRef(image.ToDataURI().c_str()), body_raw.GetAllocator());
        body_raw.AddMember("roles", role_json, body_raw.GetAllocator());

        std::string body(DumpJson(body_raw));
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPostRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

        Emoji emoji = discpp::Emoji(client, *result);
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

        std::string body("{\"name\": \"" + name + "\", \"roles\": " + json_roles + "}");
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD, body);

		discpp::Emoji resulted_emoji = discpp::Emoji(client, *result);

		auto it = emojis.find(resulted_emoji.id);
		if (it != emojis.end()) {
			it->second = resulted_emoji;
		}

		return resulted_emoji;
	}

	void Guild::DeleteEmoji(const discpp::Emoji& emoji) {
		Guild::EnsureBotPermission(Permission::MANAGE_EMOJIS);
        discpp::Client* client = GetClient();
		SendDeleteRequest(client, Endpoint("/guilds/" + std::to_string(this->id) + "/emojis/" + std::to_string(id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);

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
			return url + ".gif";
		case ImageType::JPEG:
			return url + ".jpeg";
		case ImageType::PNG:
			return url + ".png";
		case ImageType::WEBP:
			return url + ".webp";
		default:
			return url;
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
        discpp::Client* client = GetClient();

        rapidjson::Document j_body(rapidjson::kObjectType);
        for (auto request : modify_requests.guild_requests) {
            std::variant<std::string, int, Image> variant = request.second;
            std::visit(overloaded {
                    [&](int i) { j_body[GuildPropertyToString(request.first).c_str()].SetInt(i); },
                    [&](Image img) { j_body[GuildPropertyToString(request.first).c_str()].SetString(rapidjson::StringRef(img.ToDataURI().c_str())); },
                    [&](const std::string& str) { j_body[GuildPropertyToString(request.first).c_str()].SetString(rapidjson::StringRef(str.c_str())); }
            }, variant);
        }

        std::string body(DumpJson(j_body));
        std::unique_ptr<rapidjson::Document> result = SendPatchRequest(client, Endpoint("/guilds/" + std::to_string(id)), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Guild(client, *result);
        return *this;
    }

    discpp::GuildInvite Guild::GetVanityURL() const {
        discpp::Client* client = GetClient();
	    std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/vanity-url"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);

        return discpp::GuildInvite(client, *result);
    }

    discpp::AuditLog Guild::GetAuditLog() const {
        discpp::Client* client = GetClient();
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/audit-logs"), DefaultHeaders(client), id, RateLimitBucketType::GUILD);

        return discpp::AuditLog(client, *result);
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
            discpp::Client* client = GetClient();
            std::unique_ptr<rapidjson::Document> result = discpp::SendGetRequest(client, Endpoint("/guilds/" + std::to_string(id) + "/members/" + std::to_string(member_id)), DefaultHeaders(client), id, RateLimitBucketType::GUILD);

            std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(client, *result, this);
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
                return url + ".jpeg";
            case ImageType::PNG:
                return url + ".png";
            case ImageType::WEBP:
                return url + ".webp";
            default:
                return url;
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
                return url + ".jpeg";
            case ImageType::PNG:
                return url + ".png";
            case ImageType::WEBP:
                return url + ".webp";
            default:
                return url;
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
                return url + ".jpeg";
            case ImageType::PNG:
                return url + ".png";
            case ImageType::WEBP:
                return url + ".webp";
            default:
                return url;
        }
    }

    std::string Guild::GetFormattedJoinedAt() const {
        return FormatTime(std::chrono::system_clock::to_time_t(joined_at));
    }

    std::chrono::system_clock::time_point Guild::GetJoinedAt() const {
        return joined_at;
    }

    std::string Guild::GetFormattedCreatedAt() const {
        return id.GetFormattedTimestamp();
    }

    std::chrono::system_clock::time_point Guild::GetCreatedAt() const {
        return std::chrono::system_clock::from_time_t(id.GetRawTime());
    }

    void Guild::CacheMember(const std::shared_ptr<discpp::Member>& member) {
        std::lock_guard<std::mutex> lock_guard(members_mutex);
        members.emplace(member->user.id, member);
    }

    void Guild::CacheChannel(const discpp::Channel& channel) {
        std::lock_guard<std::mutex> lock_guard(channels_mutex);
        channels.emplace(channel.id, channel);
    }

    void Guild::CacheEmoji(const discpp::Emoji& emoji) {
        std::lock_guard<std::mutex> lock_guard(emojis_mutex);
        emojis.emplace(emoji.id, emoji);
    }

    GuildInvite::GuildInvite(discpp::Client* client, rapidjson::Document &json) {
        code = json["code"].GetString();
        if (ContainsNotNull(json, "guild")) {
            guild = client->cache->GetGuild(discpp::Snowflake(json["guild"]["id"].GetString()));
        }
        channel = discpp::Channel(guild->GetChannel(Snowflake(json["channel"]["id"].GetString())));
        if (ContainsNotNull(json, "inviter")) {
            rapidjson::Document inviter_json;
            inviter_json.CopyFrom(json["inviter"], inviter_json.GetAllocator());
            inviter = std::make_shared<discpp::User>(client, inviter_json);
        }
        if (ContainsNotNull(json, "target_user")) {
            rapidjson::Document target_json;
            target_json.CopyFrom(json["target_user"], target_json.GetAllocator());
            target_user = std::make_shared<discpp::User>(client, target_json);
        }
        target_user_type = static_cast<TargetUserType>(GetDataSafely<int>(json, "target_user_type"));
        approximate_presence_count = GetDataSafely<int>(json, "approximate_presence_count");
        approximate_member_count = GetDataSafely<int>(json, "approximate_member_count");
    }

    VoiceState::VoiceState(discpp::Client *client) : client(client) {

    }

    VoiceState::VoiceState(discpp::Client *client, rapidjson::Document &json) : client(client) {
		guild_id = GetIDSafely(json, "guild_id");
		channel_id = GetIDSafely(json, "channel_id");
		user_id = Snowflake(json["user_id"].GetString());
		if (ContainsNotNull(json, "member")) {
			rapidjson::Document member_json;
			member_json.CopyFrom(json["member"], member_json.GetAllocator());

			std::shared_ptr<discpp::Guild> guild = client->cache->GetGuild(guild_id);
			member = std::make_shared<discpp::Member>(client, member_json, guild);
		} else if (guild_id != 0) { // If it doesn't have member field, and has the guild_id field
            std::shared_ptr<discpp::Guild> guild = client->cache->GetGuild(guild_id);
            member = guild->GetMember(user_id);
		}
		session_id = json["session_id"].GetString();
		deaf = json["deaf"].GetBool();
		mute = json["mute"].GetBool();
		self_deaf = json["self_deaf"].GetBool();
		self_mute = json["self_mute"].GetBool();
		self_stream = GetDataSafely<bool>(json, "self_stream");
		suppress = json["suppress"].GetBool();
    }

    Integration::Integration(rapidjson::Document &json) {
        id = Snowflake(json["id"].GetString());
        name = json["name"].GetString();
        type = json["type"].GetString();
        enabled = json["enabled"].GetBool();
        syncing = json["syncing"].GetBool();
        role_id = Snowflake(json["role_id"].GetString());
        enable_emoticons = GetDataSafely<bool>(json, "enable_emoticons");
        expire_behavior = static_cast<IntegrationExpireBehavior>(json["expire_behavior"].GetInt());
        expire_grace_period = json["expire_grace_period"].GetInt();
        if (ContainsNotNull(json, "user")) {
            rapidjson::Document user_json;
            user_json.CopyFrom(json["user"], user_json.GetAllocator());

            discpp::Client* client = GetClient();
            user = std::make_shared<discpp::User>(client, user_json);
        }

        account = ConstructDiscppObjectFromJson(json, "account", discpp::IntegrationAccount());
        synced_at = json["synced_at"].GetString();
    }
}