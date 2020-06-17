//
// Created by SeanOMik on 5/3/2020.
//

#ifndef DISCPP_AUDIT_LOG_H
#define DISCPP_AUDIT_LOG_H

#include "webhook.h"
#include "user.h"

namespace discpp {
    class Integration;
	class Message;
	class Role;

    struct AuditLogChangeKey {
		/*
		 * @breif A struct that holds all possible things that changed in an AuditLog.
		 * 
		 * To see what would be valid check the discord docs: https://discordapp.com/developers/docs/resources/audit-log#audit-log-change-object-audit-log-change-key
		 *
		 */

		AuditLogChangeKey() = default;

		std::string name; /**< Name of the current audit log. */
		std::string icon_hash; /**< Icon hash of the current audit log. */
		std::string splash_hash; /**< Splash hash of the current audit log. */
		discpp::Snowflake owner_id; /**< Ownder id of current audit log. */
		std::string region; /**< Region of current audit log. */
		discpp::Snowflake afk_channel_id; /**< AFK Channel id of current audit log. */
		int afk_timeout; /* AFK Timeout of current audit log. */
		int mfa_level;
		int verification_level;
		int explicit_content_filter;
		int default_message_notifications;
		std::string vanity_url_code;
		std::vector<discpp::Role> roles_add;
		std::vector<discpp::Role> roles_remove;
		int prune_delete_days;
		bool widget_enabled;
		discpp::Snowflake widget_channel_id;
		discpp::Snowflake system_channel_id;
		int position;
		std::string topic;
		int bitrate;
		std::vector<discpp::Permissions> permission_overwrites;
		bool nsfw;
		discpp::Snowflake application_id;
		int rate_limit_per_user;
		int permissions;
		int color;
		bool hoist;
		bool mentionable;
		int allow;
		int deny;
		std::string code;
		discpp::Snowflake channel_id;
		discpp::Snowflake inviter_id;
		int max_uses;
		int uses;
		int max_age;
		bool temporary;
		bool deaf;
		bool mute;
		std::string nick;
		std::string avatar_hash;
		discpp::Snowflake id;
		std::string type;
		bool enable_emoticons;
		int expire_behavior;
		int expire_grace_period;
    };

    class AuditLogChange {
	private:
    public:
        AuditLogChange() = default;
        AuditLogChange(rapidjson::Document& json);

        std::string key;
		AuditLogChangeKey new_value;
		AuditLogChangeKey old_value;
    };

    enum class AuditLogKey : int {
        NAME, ICON_HASH, SPLASH_HASH,
        OWNER_ID, REGION, AFK_CHANNEL_ID,
        AFK_TIMEOUT, MFA_LEVEL, VERIFICATION_LEVEL,
        EXPLICIT_CONTENT_FILTER, DEFAULT_MESSAGE_NOTIFICATIONS,
        VANITY_URL_CODE, ADD, REMOVE, PRUNE_DELETE_DAYS,
        WIDGET_ENABLED, WIDGET_CHANNEL_ID, SYSTEM_CHANNEL_ID,
        POSITION, TOPIC, BITRATE, PERMISSION_OVERWRITES,
        NSFW, APPLICATION_ID, RATE_LIMIT_PER_USER,
        PERMISSIONS, COLOR, HOIST, MENTIONABLE, ALLOW,
        DENY, CODE, CHANNEL_ID, INVITER_ID, MAX_USES,
        USES, MAX_AGE, TEMPORARY, DEAF, MUTE, NICK,
        AVATAR_HASH, ID, TYPE, ENABLE_EMOTICONS,
        EXPIRE_BEHAVIOR, EXPIRE_GRACE_PERIOD
    };

    AuditLogKey StrToKey(const std::string& key) {
        std::unordered_map<std::string, AuditLogKey> tmp = {
                {"name", AuditLogKey::NAME}, {"icon_hash", AuditLogKey::ICON_HASH}, {"splash_hash", AuditLogKey::SPLASH_HASH},
                {"owner_id", AuditLogKey::OWNER_ID}, {"region", AuditLogKey::REGION}, {"afk_channel_id", AuditLogKey::AFK_CHANNEL_ID},
                {"afk_timeout", AuditLogKey::AFK_TIMEOUT}, {"mfa_level", AuditLogKey::MFA_LEVEL}, {"verification_level", AuditLogKey::VERIFICATION_LEVEL},
                {"explicit_content_filter", AuditLogKey::EXPLICIT_CONTENT_FILTER}, {"default_message_notifications", AuditLogKey::DEFAULT_MESSAGE_NOTIFICATIONS}, {"vanity_url_code", AuditLogKey::VANITY_URL_CODE},
                {"$add", AuditLogKey::ADD}, {"$remove", AuditLogKey::REMOVE}, {"prune_delete_days", AuditLogKey::PRUNE_DELETE_DAYS},
                {"widget_enabled", AuditLogKey::WIDGET_ENABLED}, {"widget_channel_id", AuditLogKey::WIDGET_CHANNEL_ID}, {"system_channel_id", AuditLogKey::SYSTEM_CHANNEL_ID},
                {"position", AuditLogKey::POSITION}, {"topic", AuditLogKey::TOPIC}, {"bitrate", AuditLogKey::BITRATE},
                {"permission_overwrites", AuditLogKey::PERMISSION_OVERWRITES}, {"nsfw", AuditLogKey::NSFW}, {"application_id", AuditLogKey::APPLICATION_ID},
                {"rate_limit_per_user", AuditLogKey::RATE_LIMIT_PER_USER}, {"permissions", AuditLogKey::PERMISSIONS}, {"color", AuditLogKey::COLOR},
                {"hoist", AuditLogKey::HOIST}, {"mentionable", AuditLogKey::MENTIONABLE}, {"allow", AuditLogKey::ALLOW},
                {"deny", AuditLogKey::DENY}, {"code", AuditLogKey::CODE}, {"channel_id", AuditLogKey::CHANNEL_ID},
                {"inviter_id", AuditLogKey::INVITER_ID}, {"max_uses", AuditLogKey::MAX_USES}, {"uses", AuditLogKey::USES},
                {"max_age", AuditLogKey::MAX_AGE}, {"temporary", AuditLogKey::TEMPORARY}, {"deaf", AuditLogKey::DEAF},
                {"mute", AuditLogKey::MUTE}, {"nick", AuditLogKey::NICK}, {"avatar_hash", AuditLogKey::AVATAR_HASH},
                {"id", AuditLogKey::ID}, {"type", AuditLogKey::TYPE}, {"enable_emoticons", AuditLogKey::ENABLE_EMOTICONS},
                {"expire_behavior", AuditLogKey::EXPIRE_BEHAVIOR}, {"expire_grace_period", AuditLogKey::EXPIRE_GRACE_PERIOD}
        };
        return tmp[key];
    }

    enum AuditLogEvent : int {
        GUILD_UPDATE = 1,
        CHANNEL_CREATE = 10,
        CHANNEL_UPDATE = 11,
        CHANNEL_DELETE = 12,
        CHANNEL_OVERWRITE_CREATE = 13,
        CHANNEL_OVERWRITE_UPDATE = 14,
        CHANNEL_OVERWRITE_DELETE = 15,
        MEMBER_KICK = 20,
        MEMBER_PRUNE = 21,
        MEMBER_BAN_ADD = 22,
        MEMBER_BAN_REMOVE = 23,
        MEMBER_UPDATE = 24,
        MEMBER_ROLE_UPDATE = 25,
        MEMBER_MOVE = 26,
        MEMBER_DISCONNECT = 27,
        BOT_ADD = 28,
        ROLE_CREATE = 30,
        ROLE_UPDATE = 31,
        ROLE_DELETE = 32,
        INVITE_CREATE = 40,
        INVITE_UPDATE = 41,
        INVITE_DELETE = 42,
        WEBHOOK_CREATE = 50,
        WEBHOOK_UPDATE = 51,
        WEBHOOK_DELETE = 52,
        EMOJI_CREATE = 60,
        EMOJI_UPDATE = 61,
        EMOJI_DELETE = 62,
        MESSAGE_DELETE = 72,
        MESSAGE_BULK_DELETE = 73,
        MESSAGE_PIN = 74,
        MESSAGE_UNPIN = 75,
        INTEGRATION_CREATE = 80,
        INTEGRATION_UPDATE = 81,
        INTEGRATION_DELETE = 82
    };

    class AuditEntryOptions {
    public:
		AuditEntryOptions() = default;
		AuditEntryOptions(rapidjson::Document& json);

        std::string delete_member_days;
        std::string members_removed;
        std::shared_ptr<discpp::Channel> channel;
		std::shared_ptr<discpp::Message> message;
        std::string count;
        discpp::Snowflake id;
        std::string type;
        std::string role_name;
    };

    class AuditLogEntry : public DiscordObject {
    public:
        AuditLogEntry() = default;
        AuditLogEntry(rapidjson::Document& json);

        std::string target_id;
        std::vector<AuditLogChange> changes;
        discpp::User user;
        AuditLogEvent action_type;
        AuditEntryOptions options;
        std::string reason;
    };

    class AuditLog {
    public:
        AuditLog() = default;
        AuditLog(rapidjson::Document& json);

        std::vector<discpp::Webhook> webhooks;
        std::vector<discpp::User> users;
        std::vector<discpp::AuditLogEntry> audit_log_entries;
        std::vector<discpp::Integration> integrations;
    };
}

#endif //DISCPP_AUDIT_LOG_H
