#ifndef DISCPP_GUILD_H
#define DISCPP_GUILD_H

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
		std::string reason; /**< Ban reason */
		discord::User user; /**< Banned member */

		GuildBan(std::string reason, discord::User user) {
			this->reason = reason;
			this->user = user;
		}
	};

	class GuildInvite {
	public:
		std::string code; /**< Guild Invite code */
		snowflake guild_id; /**< Guild ID the invite was issued for */
		discord::Channel channel; /**< Channel the invite was issued for */
		discord::User target_user; 
		int target_user_type;
		int approximate_presence_count; 
		int approximate_member_count; /**< Approximate count of members in the guild the invite was issued for */

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
		std::string id; /**< ID of the integration */
		std::string name; /**< Name of the integration */

		GuildIntegrationAccount() = default;
		GuildIntegrationAccount(nlohmann::json json) {
			id = json["id"];
			name = json["name"];
		}
	};

	class GuildIntegration : public DiscordObject {
	public:
		//snowflake id;
		std::string name; /**< Name of the integration */
		std::string type; /**< Type of the integration */
		bool enabled; /**< Is the integration enabled? */
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
		std::string name; /**< Guild name */
		std::string icon; /**< Hashed guild icon */
		std::string splash; /**< Optional guild splash */
		std::string owner; /**< Guild Owner */
		snowflake owner_id; /**< Snowflake id of the guild owner */
		Member GetOwnerObject();
		int permissions;
		std::string region; /**< Guild region */
		snowflake afk_channel_id; /**< Guild AFK voice channel if enabled */
		int afk_timeout; /**< Guild AFK timeout if enabled */
		bool embed_enabled;
		snowflake embed_channel_id;
		discord::specials::VerificationLevel verification_level; /**< Guild Verification level if enabled */
		discord::specials::DefaultMessageNotificationLevel default_message_notifications; /**< Default channel for join/leave notifications if enabled */
		discord::specials::ExplicitContentFilterLevel explicit_content_filter; /**< Content filtering level if enabled */
		std::unordered_map<snowflake, Role> roles; /**< List of roles in the current guild */
		std::unordered_map<snowflake, Emoji> emojis; /**< List of emojis in the current guild */
		// features
		discord::specials::MFALevel mfa_level; /**< MFA level for current guild */
		snowflake application_id;
		bool widget_enabled;
		snowflake widget_channel_id;
		snowflake system_channel_id;
		std::string joined_at; // TODO: Convert to iso8601Time
		bool large;
		bool unavailable;
		int member_count; /**< Number of members in the current guild */
		//voice_states
		std::unordered_map<snowflake, Member> members; /**< List of members in the current guild */
		std::unordered_map<snowflake, Channel> channels; /**< List of channels in the current guild */
		int max_presences; 
		int max_members;
		std::string vanity_url_code; /**< Optional vanity_url for current guild */
		std::string description; 
		std::string banner;
		discord::specials::NitroTier premium_tier;
		int premium_subscription_count;
		std::string preferred_locale;
		std::string created_at; /**< Date in which the current guild was created */
	};
}

#endif
