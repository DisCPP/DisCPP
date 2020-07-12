//
// Created by SeanOMik on 5/3/2020.
//

#include "audit_log.h"
#include "utils.h"
#include "user.h"
#include "guild.h"
#include "message.h"
#include "client.h"
#include "role.h"

// This is extremely ugly and probably slow, maybe theres a way we could trim this down?
discpp::AuditLogChangeKey GetKey(const std::string& key, discpp::JsonObject& j) {
	discpp::AuditLogChangeKey a_key;

	discpp::AuditLogKey keyval = discpp::StrToKey(key);

	switch(keyval) {
	    case discpp::AuditLogKey::NAME:
	        a_key.name = j.GetString();
	        break;
	    case discpp::AuditLogKey::ICON_HASH:
	        a_key.icon_hash = j.GetString();
	        break;
	    case discpp::AuditLogKey::SPLASH_HASH:
	        a_key.splash_hash = j.GetString();
	        break;
	    case discpp::AuditLogKey::OWNER_ID:
            a_key.owner_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::REGION:
            a_key.region = j.GetString();
	        break;
	    case discpp::AuditLogKey::AFK_CHANNEL_ID:
            a_key.afk_channel_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::AFK_TIMEOUT:
            a_key.afk_timeout = j.GetInt();
	        break;
	    case discpp::AuditLogKey::MFA_LEVEL:
            a_key.mfa_level = j.GetInt();
	        break;
	    case discpp::AuditLogKey::VERIFICATION_LEVEL:
            a_key.verification_level = j.GetInt();
	        break;
	    case discpp::AuditLogKey::EXPLICIT_CONTENT_FILTER:
            a_key.explicit_content_filter = j.GetInt();
	        break;
	    case discpp::AuditLogKey::DEFAULT_MESSAGE_NOTIFICATIONS:
            a_key.default_message_notifications = j.GetInt();
	        break;
	    case discpp::AuditLogKey::VANITY_URL_CODE:
            a_key.vanity_url_code = j.GetString();
	        break;
	    case discpp::AuditLogKey::ADD:
	        j.IterateThrough([&](const discpp::JsonObject& role_json)->bool {
                a_key.roles_add.emplace_back(role_json);
                return true;
	        });
	        break;
	    case discpp::AuditLogKey::REMOVE:
            j.IterateThrough([&](const discpp::JsonObject& role_json)->bool {
                a_key.roles_remove.emplace_back(role_json);
                return true;
            });
	        break;
	    case discpp::AuditLogKey::PRUNE_DELETE_DAYS:
            a_key.prune_delete_days = j.GetInt();
	        break;
	    case discpp::AuditLogKey::WIDGET_ENABLED:
            a_key.widget_enabled = j.GetBool();
	        break;
	    case discpp::AuditLogKey::WIDGET_CHANNEL_ID:
            a_key.widget_channel_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::SYSTEM_CHANNEL_ID:
            a_key.system_channel_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::POSITION:
            a_key.position = j.GetInt();
	        break;
	    case discpp::AuditLogKey::TOPIC:
            a_key.topic = j.GetString();
	        break;
	    case discpp::AuditLogKey::BITRATE:
            a_key.bitrate = j.GetInt();
	        break;
	    case discpp::AuditLogKey::PERMISSION_OVERWRITES:
            j.IterateThrough([&](const discpp::JsonObject& perm_json)->bool {
                a_key.permission_overwrites.emplace_back(perm_json);
                return true;
            });
	        break;
	    case discpp::AuditLogKey::NSFW:
            a_key.nsfw = j.GetBool();
	        break;
	    case discpp::AuditLogKey::APPLICATION_ID:
            a_key.application_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::RATE_LIMIT_PER_USER:
            a_key.rate_limit_per_user = j.GetInt();
	        break;
	    case discpp::AuditLogKey::PERMISSIONS:
            a_key.permissions = j.GetInt();
	        break;
	    case discpp::AuditLogKey::COLOR:
            a_key.color = j.GetInt();
	        break;
	    case discpp::AuditLogKey::HOIST:
            a_key.hoist = j.GetBool();
	        break;
	    case discpp::AuditLogKey::MENTIONABLE:
            a_key.mentionable = j.GetBool();
	        break;
	    case discpp::AuditLogKey::ALLOW:
            a_key.allow = j.GetBool();
	        break;
	    case discpp::AuditLogKey::DENY:
            a_key.deny = j.GetBool();
	        break;
	    case discpp::AuditLogKey::CODE:
            a_key.code = j.GetString();
	        break;
	    case discpp::AuditLogKey::CHANNEL_ID:
            a_key.channel_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::INVITER_ID:
            a_key.inviter_id = discpp::SnowflakeFromString(j.GetString());
	        break;
	    case discpp::AuditLogKey::MAX_USES:
            a_key.max_uses = j.GetInt();
	        break;
	    case discpp::AuditLogKey::USES:
            a_key.uses = j.GetInt();
	        break;
	    case discpp::AuditLogKey::MAX_AGE:
            a_key.max_age = j.GetInt();
	        break;
        case discpp::AuditLogKey::TEMPORARY:
            a_key.temporary = j.GetBool();
            break;
        case discpp::AuditLogKey::DEAF:
            a_key.deaf = j.GetBool();
            break;
        case discpp::AuditLogKey::MUTE:
            a_key.mute = j.GetBool();
            break;
        case discpp::AuditLogKey::NICK:
            a_key.nick = j.GetString();
            break;
        case discpp::AuditLogKey::AVATAR_HASH:
            break;
        case discpp::AuditLogKey::ID:
            a_key.id = discpp::SnowflakeFromString(j.GetString());
            break;
        case discpp::AuditLogKey::TYPE:
            a_key.type = j.GetString();
            break;
        case discpp::AuditLogKey::ENABLE_EMOTICONS:
            a_key.enable_emoticons = j.GetBool();
            break;
        case discpp::AuditLogKey::EXPIRE_BEHAVIOR:
            a_key.expire_behavior = j.GetInt();
            break;
        case discpp::AuditLogKey::EXPIRE_GRACE_PERIOD:
            a_key.expire_grace_period = j.GetInt();
            break;
    }

	return a_key;
}

discpp::AuditLogChange::AuditLogChange(const discpp::JsonObject& json) {
	key = json["key"].GetString();

	if (json.ContainsNotNull("new_value")) {
	    discpp::JsonObject new_value_json = json["new_value"];
		new_value = GetKey(key, new_value_json);
	}

    if (json.ContainsNotNull("old_value")) {
        discpp::JsonObject old_value_json = json["old_value"];
        old_value = GetKey(key, old_value_json);
    }
}

discpp::AuditEntryOptions::AuditEntryOptions(const discpp::JsonObject& json) {
	delete_member_days = json.Get<std::string>("delete_member_days");
	members_removed = json.Get<std::string>("members_removed");
	// @TODO: Make channel valid.
	if (json.ContainsNotNull("channel_id")) {
        channel_id = discpp::SnowflakeFromString(json["channel_id"].GetString());
	}
    if (json.ContainsNotNull("message_id")) {
        message_id = discpp::SnowflakeFromString(json["message_id"].GetString());
    }
	count = json.Get<std::string>("count");
	id = json.GetIDSafely("id");
	type = json.Get<std::string>("type");
	role_name = json.Get<std::string>("role_name");
}

discpp::AuditLogEntry::AuditLogEntry(const discpp::JsonObject& json) {
    target_id = json.Get<std::string>("target_id");
    if (json.ContainsNotNull("changes")) {
        json.IterateThrough([&] (const discpp::JsonObject& change_json)->bool {
            changes.emplace_back(change_json);
            return true;
        });
    }
    user = discpp::User(SnowflakeFromString(json["user_id"].GetString()));
    id = SnowflakeFromString(json["id"].GetString());
    action_type = static_cast<discpp::AuditLogEvent>(json["action_type"].GetInt());
    options = json.ConstructDiscppObjectFromJson("options", discpp::AuditEntryOptions());
    reason = json.Get<std::string>("reason");
}

discpp::AuditLog::AuditLog(const discpp::JsonObject& json) {
    json["webhooks"].IterateThrough([&] (const discpp::JsonObject& webhook_json)->bool {
        webhooks.emplace_back(webhook_json);
        return true;
    });

    json["user"].IterateThrough([&] (const discpp::JsonObject& user_json)->bool {
        users.emplace_back(user_json);
        return true;
    });

    json["audit_log_entries"].IterateThrough([&] (const discpp::JsonObject& audit_log_entry_json)->bool {
        audit_log_entries.emplace_back(audit_log_entry_json);
        return true;
    });

    json["integrations"].IterateThrough([&] (const discpp::JsonObject& integration_json)->bool {
        integrations.emplace_back(integration_json);
        return true;
    });
}