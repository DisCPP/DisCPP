#ifndef DISCORDPP_GUILD_H
#define DISCORDPP_GUILD_H

#include "discord_object.h"
#include "emoji.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "role.h"
#include "image.h"

#include <nlohmann/json.hpp>

#include <optional>

namespace discord {
	class Guild;
	class User;

	struct GuildBan {
		std::string reason;
		discord::User user;

		GuildBan(std::string reason, discord::User user) {
			this->reason = reason;
			this->user = user;
		}
	};

	class GuildInvite {
	public:
		std::string code;
		snowflake guild_id;
		discord::Channel channel;
		discord::User target_user;
		int target_user_type;
		int approximate_presence_count;
		int approximate_member_count;

		GuildInvite() = default;
		GuildInvite(nlohmann::json json) {
			code = json["code"];
			guild_id = (json.contains("guild")) ? json["guild"]["id"].get<snowflake>() : 0;
			channel = discord::Channel(json["channel"]["id"].get<snowflake>());
			target_user = (json.contains("target_user")) ? discord::User(json["target_user"]) : discord::User();
			target_user_type = GetDataSafely<int>(json, "target_user_type");
			approximate_presence_count = GetDataSafely<int>(json, "approximate_presence_count");
			approximate_member_count = GetDataSafely<int>(json, "approximate_member_count");
		}
	};

	struct GuildIntegrationAccount {
		std::string id;
		std::string name;

		GuildIntegrationAccount() = default;
		GuildIntegrationAccount(nlohmann::json json) {
			id = json["id"];
			name = json["name"];
		}
	};

	class GuildIntegration : public DiscordObject {
	public:
		//snowflake id;
		std::string name;
		std::string type;
		bool enabled;
		bool syncing;
		discord::Role role;
		int expire_behavior;
		int expire_grace_period;
		discord::User user;
		discord::GuildIntegrationAccount account;
		std::string synced_at; // TODO: Convert to iso8601Time

		GuildIntegration() = default;
		GuildIntegration(nlohmann::json json) {
			id = json["id"].get<snowflake>();
			name = json["name"];
			type = json["type"];
			enabled = json["enabled"].get<bool>();
			syncing = json["syncing"].get<bool>();
			role = discord::Role(json["role_id"].get<snowflake>());
			expire_behavior = json["expire_behavior"].get<int>();
			expire_grace_period = json["expire_grace_period"].get<int>();
			user = discord::User(json["user"]);
			account = discord::GuildIntegrationAccount(json["account"]);
			synced_at = json["synced_at"];
		}
	};

	class GuildEmbed : public DiscordObject {
	public:
		snowflake channel_id;
		bool enabled;

		GuildEmbed() = default;
		GuildEmbed(nlohmann::json json) {
			channel_id = json["channel_id"].get<snowflake>();
			enabled = json["enabled"].get<bool>();
		}
	};

	enum WidgetStyle : int {
		SHIELD,
		BANNER1,
		BANNER2,
		BANNER3,
		BANNER4
	};

	enum GuildChannelType : int {
		GUILD_TEXT,
		DM,
		GUILD_VOICE,
		GROUP_DM,
		GROUP_CATEGORY,
		GROUP_NEWS,
		GROUP_STORE
	};

	class Guild : public DiscordObject {
	public:
		Guild() = default;
		Guild(snowflake id);
		Guild(nlohmann::json json);

		discord::Guild ModifyGuildName(std::string name);
		discord::Guild ModifyGuildRegion(discord::snowflake region_id);
		discord::Guild ModifyGuildVerificationLevel(discord::specials::VerificationLevel verification_level);
		discord::Guild ModifyGuildDefaultMessageNotifications(discord::specials::DefaultMessageNotificationLevel notification_level);
		discord::Guild ModifyGuildExplicitContentFilter(discord::specials::ExplicitContentFilterLevel explicit_content_filter);
		discord::Guild ModifyGuildAFKChannelID(discord::snowflake afk_channel_id);
		discord::Guild ModifyGuildAFKTimeout(int timeout);
		// discord::Guild ModifyGuild(icon); // https://discordapp.com/developers/docs/resources/guild#modify-guild
		discord::Guild ModifyGuildOwnerID(discord::snowflake owner_id);
		// discord::Guild ModifyGuildSplash(); // https://discordapp.com/developers/docs/resources/guild#modify-guild
		// discord::Guild ModifyGuildBanner(); // https://discordapp.com/developers/docs/resources/guild#modify-guild
		discord::Guild ModifyGuildSystemChannelID(discord::snowflake system_channel_id);
		discord::Guild ModifyGuildRulesChannelID(discord::snowflake rules_channel_id);
		discord::Guild ModifyGuildPublicUpdatesChannelID(discord::snowflake public_updates_channel_id);
		discord::Guild ModifyGuildPreferredLocale(std::string preferred_locale);

		void DeleteGuild();
		std::vector<discord::Channel> GetChannels();
		discord::Channel CreateChannel(std::string name, std::string topic = "", GuildChannelType type = GuildChannelType::GUILD_TEXT, int bitrate = 0, int user_limit = 0, int rate_limit_per_user = 0, int position = 0, std::vector<discord::Permissions> permission_overwrites = {}, discord::Channel category = {}, bool nsfw = false);
		void ModifyChannelPositions(std::vector<discord::Channel> new_channel_positions);
		discord::Member GetMember(snowflake id);
		discord::Member AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf);
		void RemoveMember(discord::Member member);
		std::vector<discord::GuildBan> GetBans();
		std::optional<std::string> GetMemberBanReason(discord::Member member);
		void BanMember(discord::Member member, std::string reason = "");
		void UnbanMember(discord::Member member);
		void KickMember(discord::Member member);
		discord::Role GetRole(snowflake id);
		discord::Role CreateRole(std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		void ModifyRolePositions(std::vector<discord::Role> new_role_positions); // https://discordapp.com/developers/docs/resources/guild#modify-guild-role-positions
		discord::Role ModifyRole(discord::Role role, std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		void DeleteRole(discord::Role role);
		int GetPruneAmount(int days);
		void BeginPrune(int days);
		std::vector<discord::GuildInvite> GetInvites();
		std::vector<discord::GuildIntegration> GetIntegrations();
		void CreateIntegration(snowflake id, std::string type);
		void ModifyIntegration(discord::GuildIntegration guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons);
		void DeleteIntegration(discord::GuildIntegration guild_integration);
		void SyncIntegration(discord::GuildIntegration guild_integration);
		GuildEmbed GetGuildEmbed();
		GuildEmbed ModifyGuildEmbed(snowflake channel_id, bool enabled);
		// discord::GuildInvite GetVanityURL(); // https://discordapp.com/developers/docs/resources/guild#get-guild-vanity-url - Doesn't work.
		std::string GetWidgetImageURL(WidgetStyle widget_style = WidgetStyle::SHIELD);

		std::unordered_map<snowflake, Emoji> GetEmojis();
		discord::Emoji GetEmoji(snowflake id);
		discord::Emoji CreateEmoji(std::string name, discord::Image image, std::vector<discord::Role> roles);
		discord::Emoji ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles);
		void DeleteEmoji(discord::Emoji emoji);
		std::string GetIconURL(discord::ImageType imgType = discord::ImageType::AUTO);

		//snowflake id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner;
		snowflake owner_id;
		Member GetOwnerObject();
		int permissions;
		std::string region;
		snowflake afk_channel_id;
		int afk_timeout;
		bool embed_enabled;
		snowflake embed_channel_id;
		discord::specials::VerificationLevel verification_level;
		discord::specials::DefaultMessageNotificationLevel default_message_notifications;
		discord::specials::ExplicitContentFilterLevel explicit_content_filter;
		std::unordered_map<snowflake, Role> roles;
		std::unordered_map<snowflake, Emoji> emojis;
		// features
		discord::specials::MFALevel mfa_level;
		snowflake application_id;
		bool widget_enabled;
		snowflake widget_channel_id;
		snowflake system_channel_id;
		std::string joined_at; // TODO: Convert to iso8601Time
		bool large;
		bool unavailable;
		int member_count;
		//voice_states
		std::unordered_map<snowflake, Member> members;
		std::unordered_map<snowflake, Channel> channels;
		int max_presences;
		int max_members;
		std::string vanity_url_code;
		std::string description;
		std::string banner;
		discord::specials::NitroTier premium_tier;
		int premium_subscription_count;
		std::string preferred_locale;
		std::string created_at;
	};
}

#endif
