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
#include <utility>

namespace discpp {
	class Guild;
	class User;

	struct GuildBan {
		std::string reason; /**< Ban reason */
		discpp::User user; /**< Banned member */

		GuildBan(std::string reason, discpp::User user) {
			this->reason = reason;
			this->user = user;
		}
	};

	class GuildInvite {
	public:
		std::string code; /**< Guild Invite code */
		snowflake guild_id; /**< Guild ID the invite was issued for */
		discpp::Channel channel; /**< Channel the invite was issued for */
		discpp::User target_user; 
		int target_user_type;
		int approximate_presence_count; 
		int approximate_member_count; /**< Approximate count of members in the guild the invite was issued for */

		GuildInvite() = default;
		GuildInvite(nlohmann::json json) {
			code = json["code"];
			guild_id = (json.contains("guild")) ? json["guild"]["id"].get<snowflake>() : 0;
			channel = discpp::Channel(json["channel"]["id"].get<snowflake>());
			target_user = (json.contains("target_user")) ? discpp::User(json["target_user"]) : discpp::User();
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
		discpp::Role role; 
		int expire_behavior;
		int expire_grace_period;
		discpp::User user;
		discpp::GuildIntegrationAccount account;
		std::string synced_at; // TODO: Convert to iso8601Time

		GuildIntegration() = default;
		GuildIntegration(nlohmann::json json) {
			id = json["id"].get<snowflake>();
			name = json["name"];
			type = json["type"];
			enabled = json["enabled"].get<bool>();
			syncing = json["syncing"].get<bool>();
			role = discpp::Role(json["role_id"].get<snowflake>());
			expire_behavior = json["expire_behavior"].get<int>();
			expire_grace_period = json["expire_grace_period"].get<int>();
			user = discpp::User(json["user"]);
			account = discpp::GuildIntegrationAccount(json["account"]);
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

	class VoiceState {
	public:
	    VoiceState() = default;
	    VoiceState(nlohmann::json json) {
            guild_id = GetDataSafely<snowflake>(json, guild_id);
            channel_id = GetDataSafely<snowflake>(json, guild_id);
            user_id = json[guild_id].get<snowflake>();
            if (json.contains("member")) {
                member = discpp::Member(json["member"]);
            }
            session_id = json["session_id"];
            deaf = json["deaf"].get<bool>();
            deaf = json["mute"].get<bool>();
            deaf = json["self_deaf"].get<bool>();
            deaf = json["self_mute"].get<bool>();
            deaf = json["self_stream"].get<bool>();
            deaf = json["suppress"].get<bool>();
	    }

	    snowflake guild_id; /**< The guild id this voice state is for. */
	    snowflake channel_id; /**< The channel id this user is connected to. */
	    snowflake user_id; /**< The user id this voice state is for. */
	    discpp::Member member; /**< The guild member this voice state is for. */
	    std::string session_id; /**< The session id for this voice state. */
	    bool deaf; /**< Whether this user is deafened by the server. */
	    bool mute; /**< Whether this user is muted by the server. */
	    bool self_deaf; /**< Whether this user is locally deafened. */
	    bool self_mute; /**< Whether this user is locally muted. */
	    bool self_stream; /**< Whether this user is streaming using "Go Live". */
	    bool suppress; /**< Whether this user is muted by the current user. */
	};

    enum class GuildProperty : int {
        NAME,
        REGION,
        VERIFICATION_LEVEL,
        DEFAULT_MESSAGE_NOTIFICATIONS,
        EXPLICIT_CONTENT_FILTER,
        AFK_CHANNEL_ID,
        AFK_TIMEOUT,
        ICON,
        OWNER_ID,
        SPLASH,
        BANNER,
        SYSTEM_CHANNEL_ID,
        RULES_CHANNEL_ID,
        PUBLIC_UPDATES_CHANNEL_ID,
        PREFERRED_LOCALE
    };

    struct GuildModifyRequests {
        std::unordered_map<GuildProperty, std::variant<std::string, int, Image>> guild_requests;

        GuildModifyRequests(GuildProperty key, const std::variant<std::string, int, Image>& value ) : guild_requests({{ key, value }}) {};
        GuildModifyRequests(std::unordered_map<GuildProperty, std::variant<std::string, int, Image>> guild_requests) : guild_requests(std::move(guild_requests)) {};

        void Add(GuildProperty key, const std::variant<std::string, int, Image>& value) {
            guild_requests.insert({key, value});
        };

        void Remove(GuildProperty key) {
            guild_requests.erase(guild_requests.find(key));
        }
    };

	class Guild : public DiscordObject {
	public:
		Guild() = default;
		Guild(snowflake id);
		Guild(nlohmann::json json);

		discpp::Guild Modify(GuildModifyRequests modify_requests);
		void DeleteGuild();
		std::vector<discpp::Channel> GetChannels();
		discpp::Channel CreateChannel(std::string name, std::string topic = "", ChannelType type = ChannelType::GUILD_TEXT, int bitrate = 0, int user_limit = 0, int rate_limit_per_user = 0, int position = 0, std::vector<discpp::Permissions> permission_overwrites = {}, discpp::Channel category = {}, bool nsfw = false);
		void ModifyChannelPositions(std::vector<discpp::Channel> new_channel_positions);
		discpp::Member GetMember(snowflake id);
		void EnsureBotPermission(Permission reqPerm);
		discpp::Member AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discpp::Role> roles, bool mute, bool deaf);
		void RemoveMember(discpp::Member member);
		std::vector<discpp::GuildBan> GetBans();
		std::optional<std::string> GetMemberBanReason(discpp::Member member);
		void BanMember(discpp::Member member, std::string reason = "");
		void UnbanMember(discpp::Member member);
		void KickMember(discpp::Member member);
		discpp::Role GetRole(snowflake id);
		discpp::Role CreateRole(std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		void ModifyRolePositions(std::vector<discpp::Role> new_role_positions);
		discpp::Role ModifyRole(discpp::Role role, std::string name, Permissions permissions = Permissions(), int color = 0, bool hoist = false, bool mentionable = false);
		void DeleteRole(discpp::Role role);
		int GetPruneAmount(int days);
		void BeginPrune(int days);
		std::vector<discpp::GuildInvite> GetInvites();
		std::vector<discpp::GuildIntegration> GetIntegrations();
		void CreateIntegration(snowflake id, std::string type);
		void ModifyIntegration(discpp::GuildIntegration guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons);
		void DeleteIntegration(discpp::GuildIntegration guild_integration);
		void SyncIntegration(discpp::GuildIntegration guild_integration);
		GuildEmbed GetGuildEmbed();
		GuildEmbed ModifyGuildEmbed(snowflake channel_id, bool enabled);
		discpp::GuildInvite GetVanityURL();
		std::string GetWidgetImageURL(WidgetStyle widget_style = WidgetStyle::SHIELD);

		std::unordered_map<snowflake, Emoji> GetEmojis();
		discpp::Emoji GetEmoji(snowflake id);
		discpp::Emoji CreateEmoji(std::string name, discpp::Image image, std::vector<discpp::Role> roles);
		discpp::Emoji ModifyEmoji(discpp::Emoji emoji, std::string name, std::vector<discpp::Role> roles);
		void DeleteEmoji(discpp::Emoji emoji);
		std::string GetIconURL(discpp::ImageType imgType = discpp::ImageType::AUTO);
		discpp::Member GetOwnerObject();

		std::string name; /**< Guild name. */
		std::string icon; /**< Hashed guild icon. */
		std::string splash; /**< Optional hashed guild splash. */
		std::string discovery_splash; /**< Optional hashed discovery splash. */
		std::string owner; /**< Whether or not the bot is the owner of the guild. */
		snowflake owner_id; /**< ID of the guild owner. */
		int permissions; /**< Total permissions for the bot in the guild (does not include channel overrides). */
		std::string region; /**< Voice region id for the guild. */
		snowflake afk_channel_id; /**< ID of afk channel. */
		int afk_timeout; /**< AFK timeout in seconds. */
		bool embed_enabled;/**< Whether this guild is embeddable (e.g. widget). */
		snowflake embed_channel_id;/**< If not null, the channel id that the widget will generate an invite to. */
		discpp::specials::VerificationLevel verification_level; /**< Verification level required for the guild. */
		discpp::specials::DefaultMessageNotificationLevel default_message_notifications; /**< Default message notifications level. */
		discpp::specials::ExplicitContentFilterLevel explicit_content_filter; /**< Explicit content filter level. */
		std::unordered_map<snowflake, Role> roles; /**< Roles in the guild. */
		std::unordered_map<snowflake, Emoji> emojis; /**< Custom guild emojis. */
		std::vector<std::string> features; /**< Enabled guild features. */
		discpp::specials::MFALevel mfa_level; /**< Required MFA level for the guild. */
		snowflake application_id; /**< Application id of the guild creator if it is bot-created. */
		bool widget_enabled; /**< Whether or not the server widget is enabled. */
		snowflake widget_channel_id; /**< The channel id for the server widget. */
		snowflake system_channel_id; /**< The id of the channel where guild notices such as welcome messages and boost events are posted. */
        int system_channel_flags; /**< System channel flags. */
        snowflake rules_channel_id; /**< The id of the channel where "PUBLIC" guilds display rules and/or guidelines. */
		// @TODO: Convert to iso8601Time
		std::string joined_at; /**< When this guild was joined at. */
		bool large; /**< Whether this is considered a large guild. */
		bool unavailable; /**< Whether this guild is unavailable. */
		int member_count; /**< Total number of members in this guild. */
		std::vector<discpp::VoiceState> voice_states; /**< Array of partial voice state objects. */
		std::unordered_map<snowflake, Member> members; /**< Users in the guild. */
		std::unordered_map<snowflake, Channel> channels; /**< Channels in the guild. */
		int max_presences; /**< The maximum amount of presences for the guild (the default value, currently 25000, is in effect when null is returned). */
		int max_members; /**< The maximum amount of members for the guild. */
		std::string vanity_url_code; /**< The vanity url code for the guild. */
		std::string description; /**< The description for the guild. */
		std::string banner; /**< Banner hash. */
		discpp::specials::NitroTier premium_tier; /**< Premium tier (Server Boost level). */
		int premium_subscription_count; /**< The number of boosts this server currently has. */
		std::string preferred_locale; /**< The preferred locale of a "PUBLIC" guild used in server discovery and notices from Discord; defaults to "en-US". */
		std::string created_at; /**< The id of the channel where admins and moderators of "PUBLIC" guilds receive notices from Discord. */
	};
}

#endif
