#include "guild.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "emoji.h"
#include "bot.h"

namespace discord {
	Guild::Guild(snowflake id) {
		auto guild = std::find_if(discord::globals::bot_instance->guilds.begin(), discord::globals::bot_instance->guilds.end(), [id](discord::Guild a) { return id == a.id; });

		if (guild != discord::globals::bot_instance->guilds.end()) {
			*this = *guild;
		}
	}

	Guild::Guild(nlohmann::json json) {
		id = GetDataSafely<snowflake>(json, "id");
		name = GetDataSafely<std::string>(json, "name");
		icon = GetDataSafely<std::string>(json, "icon");
		splash = GetDataSafely<std::string>(json, "splash");
		owner = GetDataSafely<bool>(json, owner);
		owner_id = GetDataSafely<snowflake>(json, "owner_id");
		permissions = GetDataSafely<int>(json, "permissions");
		region = GetDataSafely<std::string>(json, "region");
		afk_channel_id = GetDataSafely<snowflake>(json, "afk_channel_id");
		afk_timeout = GetDataSafely<int>(json, "afk_timeout");
		embed_enabled = GetDataSafely<bool>(json, "embed_enabled");
		embed_channel_id = GetDataSafely<snowflake>(json, "embed_channel_id");
		verification_level = (json.contains("verification_level")) ? static_cast<discord::specials::VerificationLevel>(json["verification_level"].get<int>()) : discord::specials::VerificationLevel::NO_VERIFICATION;
		default_message_notifications = (json.contains("default_message_notifications")) ? static_cast<discord::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].get<int>()) : discord::specials::DefaultMessageNotificationLevel::ALL_MESSAGES;
		explicit_content_filter = (json.contains("explicit_content_filter")) ? static_cast<discord::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].get<int>()) : discord::specials::ExplicitContentFilterLevel::DISABLED;
		if (json.contains("roles")) {
			for (auto& role : json["roles"]) {
				roles.push_back(discord::Role(role));
			}
		}
		if (json.contains("emojis")) {
			for (auto& emoji : json["emojis"]) {
				emojis.push_back(discord::Emoji(emoji));
			}
		}
		// features
		mfa_level = (json.contains("mfa_level")) ? static_cast<discord::specials::MFALevel>(json["mfa_level"].get<int>()) : discord::specials::MFALevel::NO_MFA;
		application_id = GetDataSafely<snowflake>(json, "application_id");
		widget_enabled = GetDataSafely<bool>(json, "widget_enabled");
		widget_channel_id = GetDataSafely<snowflake>(json, "widget_channel_id");
		system_channel_id = GetDataSafely<snowflake>(json, "system_channel_id");
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		large = GetDataSafely<bool>(json, "large");
		unavailable = GetDataSafely<bool>(json, "unavailable");
		member_count = GetDataSafely<int>(json, "member_count");
		// voice_states
		if (json.contains("members")) {
			for (auto& member : json["members"]) {
				members.push_back(discord::Member(member, id));
			}
		}
		if (json.contains("channels")) {
			for (auto& channel : json["channels"]) {
				channels.push_back(discord::Channel(channel));
			}
		}
		// presences
		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");
		vanity_url_code = GetDataSafely<std::string>(json, "vanity_url_code");
		description = GetDataSafely<std::string>(json, "description");
		banner = GetDataSafely<std::string>(json, "banner");
		premium_tier = (json.contains("premium_tier")) ? static_cast<discord::specials::NitroTier>(json["premium_tier"].get<int>()) : discord::specials::NitroTier::NO_TIER;
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = GetDataSafely<std::string>(json, "preferred_locale");
	}

	void Guild::DeleteGuild() {
		SendDeleteRequest(Endpoint("/guilds/%", id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discord::Channel> Guild::GetChannels() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/channels", id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		std::vector<discord::Channel> channels;
		for (auto& channel : result) {
			channels.push_back(discord::Channel(channel));
		}
		return channels;
	}

	discord::Channel Guild::CreateChannel(std::string name, GuildChannelType type, std::string topic, int bitrate, int user_limit, int rate_limit_per_user, int position, std::vector<discord::Permissions> permission_overwrites, discord::Channel category, bool nsfw) {
		if (bitrate < 8000) bitrate = 8000;

		nlohmann::json permission_json = nlohmann::json::array();
		for (auto perm : permission_overwrites) {
			permission_json.push_back(perm.ToJson());
		}

		nlohmann::json json_raw = nlohmann::json({
			{"name", name},
			{"type", type},
			{"topic", topic},
			{"bitrate", bitrate},
			{"user_limit", user_limit},
			{"rate_limit_per_user", rate_limit_per_user},
			{"position", position},
			{"permission_overwrites", permission_json},
			{"parent_id", category.id},
			{"nsfw", nsfw}
		});

		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/%/channels", id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::CHANNEL, body);
		discord::Channel channel(result);
		channels.push_back(channel);

		return channel;
	}

	discord::Member Guild::GetMember(snowflake id) {
		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			return *member;
		}
		throw std::runtime_error("Member not found!");
	}

	discord::Member Guild::AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf) {
		std::string json_roles = "[";
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			} else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");
		nlohmann::json result = SendPutRequest(Endpoint("/guilds/%/members/%", this->id, id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		return (result == "{}") ? discord::Member(id) : discord::Member(result, id); // If the member is already added, return it.
	}

	void Guild::AddRoleToMember(discord::Member member, discord::Role role) {
		SendPutRequest(Endpoint("/guilds/" + id + "/members/" + member.user.id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::RemoveRoleToMember(discord::Member member, discord::Role role) {
		SendDeleteRequest(Endpoint("/guilds/" + id + "/members/" + member.user.id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::RemoveMember(discord::Member member) {
		SendDeleteRequest(Endpoint("/guilds/" + id + "/members/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discord::GuildBan> Guild::GetBans() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);

		std::vector<discord::GuildBan> guild_bans;
		for (auto& guild_ban : result) {
			guild_bans.push_back(discord::GuildBan(guild_ban["reason"], discord::User(guild_ban["user"])));
		}

		return guild_bans;
	}

	std::optional<std::string> Guild::GetMemberBanReason(discord::Member member) {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/bans/%", id, member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (result.contains("reason")) return result["reason"];
		return std::nullopt;
	}

	bool Guild::IsMemberBanned(discord::Member member) {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/bans/%", id, member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return result.contains("reason");
	}

	void Guild::BanMember(discord::Member member, std::string reason) {
		cpr::Body body(Format("{\"reason\": \"%\"}", reason));
		SendPutRequest(Endpoint("/guilds/%/bans/%", id, member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::UnbanMember(discord::Member member) {
		SendDeleteRequest(Endpoint("/guilds/%/bans/%", id, member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discord::Role Guild::CreateRole(std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		nlohmann::json json_body = nlohmann::json({
			{"name", name},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/%/roles", id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Role new_role(result);
		roles.push_back(new_role);

		return new_role;
	}

	discord::Role Guild::ModifyRole(discord::Role role, std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		nlohmann::json json_body = nlohmann::json({
			{"name", name},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/%/roles/%", id, role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Role modified_role(result);
		std::replace_if(roles.begin(), roles.end(), [modified_role](discord::Role r) { return r.id == modified_role.id; }, modified_role);

		return modified_role;
	}

	void Guild::DeleteRole(discord::Role role) {
		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/%/roles/%", id, role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		roles.erase(std::remove_if(roles.begin(), roles.end(), [role](discord::Role r) { return r.id == role.id; }));
	}

	std::vector<discord::GuildInvite> Guild::GetInvites() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/invites", id), DefaultHeaders(), {}, {});

		std::vector<discord::GuildInvite> guild_invites;
		for (auto& guild_invite : result) {
			guild_invites.push_back(discord::GuildInvite(guild_invite));
		}

		return guild_invites;
	}

	std::vector<discord::GuildIntegration> Guild::GetIntegrations() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/integrations", id), DefaultHeaders(), {}, {});

		std::vector<discord::GuildIntegration> guild_integrations;
		for (auto& guild_integration : result) {
			guild_integrations.push_back(discord::GuildIntegration(guild_integration));
		}

		return guild_integrations;
	}

	void Guild::CreateIntegration(snowflake id, std::string type) {
		cpr::Body body(Format("{\"type\": \"%\", \"id\": \"%\"}", type, id));
		SendPostRequest(Endpoint("/guilds/%/integrations", this->id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(discord::GuildIntegration guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons) {
		cpr::Body body(Format("{\"expire_behavior\": %, \"expire_grace_period\": %, \"enable_emoticons\": %}", expire_behavior, expire_grace_period, enable_emoticons));
		SendPostRequest(Endpoint("/guilds/%/integrations/%", id, guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::DeleteIntegration(discord::GuildIntegration guild_integration) {
		SendDeleteRequest(Endpoint("/guilds/%/integrations/%", id, guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::SyncIntegration(discord::GuildIntegration guild_integration) {
		SendPostRequest(Endpoint("/guilds/%/integrations/%/sync", id, guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discord::GuildEmbed Guild::GetGuildEmbed() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/embed", id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return discord::GuildEmbed(result);
	}

	discord::GuildEmbed Guild::ModifyGuildEmbed(snowflake channel_id, bool enabled) {
		cpr::Body body(Format("{\"channel_id\": \"%\", \"enabled\": %}", channel_id, enabled));
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/%/embed", id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discord::GuildEmbed();
	}

	std::string Guild::GetWidgetImageURL(WidgetStyle widget_style) {
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
		cpr::Body body(Format("{\"style\": %}", style));
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/widget.png", id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::string();
	}

	std::vector<discord::Emoji> Guild::GetEmojis() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/emojis", id), DefaultHeaders(), {}, {});

		std::vector<discord::Emoji> emojis;
		for (auto& emoji : result) {
			emojis.push_back(discord::Emoji(emoji));
		}
		this->emojis = emojis;

		return emojis;
	}

	discord::Emoji Guild::GetEmoji(snowflake id) {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/%/emojis/%", this->id, id), DefaultHeaders(), {}, {});

		return discord::Emoji(result);
	}

	discord::Emoji Guild::ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles) {
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

		cpr::Body body(Format("{\"name\": \"%\", \"roles\": %}", json_roles));
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/%/emojis/%", this->id, id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Emoji em(result);
		std::replace_if(emojis.begin(), emojis.end(), [em](discord::Emoji e) { return e.id == em.id; }, em);

		return em;
	}

	void Guild::DeleteEmoji(discord::Emoji emoji) {
		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/%/emojis/%", this->id, id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		emojis.erase(std::remove_if(emojis.begin(), emojis.end(), [emoji](discord::Emoji e) { return e.id == emoji.id; }));
	}
}