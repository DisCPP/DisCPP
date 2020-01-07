#ifndef DISCORDPP_GUILD_H
#define DISCORDPP_GUILD_H

#include "discord_object.h"
#include "emoji.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "role.h"

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

	class GuildIntegration : DiscordObject {
	public:
		snowflake id;
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

	class GuildEmbed : DiscordObject {
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

	class Guild : DiscordObject {
	public:
		Guild() = default;
		Guild(snowflake id);
		Guild(nlohmann::json json);

		// discord::Guild ModifyGuild(); // https://discordapp.com/developers/docs/resources/guild#modify-guild
		void DeleteGuild();
		std::vector<discord::Channel> GetChannels();
		discord::Channel CreateChannel(std::string name, GuildChannelType type, std::string topic, int bitrate, int user_limit, int rate_limit_per_user, int position, std::vector<discord::Permissions> permission_overwrites, discord::Channel category, bool nsfw);
		// void ModifyChannelPositions(std::vector<discord::Channel> channels, std::vector<int> positiion); // https://discordapp.com/developers/docs/resources/guild#modify-guild-channel-positions
		discord::Member GetMember(snowflake id);
		discord::Member AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf);
		void AddRoleToMember(discord::Member member, discord::Role role);
		void RemoveRoleToMember(discord::Member member, discord::Role role);
		void RemoveMember(discord::Member member);
		std::vector<discord::GuildBan> GetBans();
		std::optional<std::string> GetMemberBanReason(discord::Member member);
		bool IsMemberBanned(discord::Member member);
		void BanMember(discord::Member member, std::string reason = "");
		void UnbanMember(discord::Member member);
		//std::vector<discord::Role> GetRoles(); // Not needed due to variable
		discord::Role CreateRole(std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		// std::vector<discord::Role> ModifyRolePositions(std::vector<snowflake> ids, std::vector<int> positions); // https://discordapp.com/developers/docs/resources/guild#modify-guild-role-positions
		discord::Role ModifyRole(discord::Role role, std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		void DeleteRole(discord::Role role);
		// void GetPruneAmount(); // https://discordapp.com/developers/docs/resources/guild#get-guild-prune-count
		// void BeginPrune(int days, bool comptute_prune_count = false); // https://discordapp.com/developers/docs/resources/guild#begin-guild-prune
		std::vector<discord::GuildInvite> GetInvites();
		std::vector<discord::GuildIntegration> GetIntegrations();
		void CreateIntegration(snowflake id, std::string type);
		void ModifyIntegration(discord::GuildIntegration guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons);
		void DeleteIntegration(discord::GuildIntegration guild_integration);
		void SyncIntegration(discord::GuildIntegration guild_integration);
		GuildEmbed GetGuildEmbed();
		GuildEmbed ModifyGuildEmbed(snowflake channel_id, bool enabled);
		// void GetVanityURL() // https://discordapp.com/developers/docs/resources/guild#get-guild-vanity-url
		std::string GetWidgetImageURL(WidgetStyle widget_style = WidgetStyle::SHIELD);

		std::vector<discord::Emoji> GetEmojis();
		discord::Emoji GetEmoji(snowflake id);
		// discord::Emoji CreateEmoji(std::string name, std::string image, std::vector<discord::Role> roles); // https://discordapp.com/developers/docs/resources/emoji#create-guild-emoji
		discord::Emoji ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles); // https://discordapp.com/developers/docs/resources/emoji#modify-guild-emoji
		void DeleteEmoji(discord::Emoji emoji);

		snowflake id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner;
		snowflake owner_id;
		int permissions;
		std::string region;
		snowflake afk_channel_id;
		int afk_timeout;
		bool embed_enabled;
		snowflake embed_channel_id;
		discord::specials::VerificationLevel verification_level;
		discord::specials::DefaultMessageNotificationLevel default_message_notifications;
		discord::specials::ExplicitContentFilterLevel explicit_content_filter;
		std::vector<discord::Role> roles;
		std::vector<discord::Emoji> emojis;
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
		std::vector<discord::Member> members;
		std::vector<discord::Channel> channels;
		//presences
		int max_presences;
		int max_members;
		std::string vanity_url_code;
		std::string description;
		std::string banner;
		discord::specials::NitroTier premium_tier;
		int premium_subscription_count;
		std::string preferred_locale;
	};
}

#endif