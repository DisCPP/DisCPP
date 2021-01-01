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
discpp::AuditLogChangeKey GetKey(discpp::Client* client, const std::string& key, rapidjson::Document& j) {
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
            a_key.owner_id = discpp::Snowflake(j.GetString());
	        break;
	    case discpp::AuditLogKey::REGION:
            a_key.region = j.GetString();
	        break;
	    case discpp::AuditLogKey::AFK_CHANNEL_ID:
            a_key.afk_channel_id = discpp::Snowflake(j.GetString());
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
            for (auto const& role : j.GetArray()) {
                rapidjson::Document role_json(rapidjson::kObjectType);
                role_json.CopyFrom(role, role_json.GetAllocator());

                a_key.roles_add.push_back(discpp::Role(client, role_json));
            }
	        break;
	    case discpp::AuditLogKey::REMOVE:
            for (auto const& role : j.GetArray()) {
                rapidjson::Document role_json(rapidjson::kObjectType);
                role_json.CopyFrom(role, role_json.GetAllocator());

                a_key.roles_remove.push_back(discpp::Role(client, role_json));
            }
	        break;
	    case discpp::AuditLogKey::PRUNE_DELETE_DAYS:
            a_key.prune_delete_days = j.GetInt();
	        break;
	    case discpp::AuditLogKey::WIDGET_ENABLED:
            a_key.widget_enabled = j.GetBool();
	        break;
	    case discpp::AuditLogKey::WIDGET_CHANNEL_ID:
            a_key.widget_channel_id = discpp::Snowflake(j.GetString());
	        break;
	    case discpp::AuditLogKey::SYSTEM_CHANNEL_ID:
            a_key.system_channel_id = discpp::Snowflake(j.GetString());
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
            for (auto const& perm : j.GetArray()) {
                rapidjson::Document perm_json(rapidjson::kObjectType);
                perm_json.CopyFrom(perm, perm_json.GetAllocator());

                a_key.permission_overwrites.push_back(discpp::Permissions(perm_json));
            }
	        break;
	    case discpp::AuditLogKey::NSFW:
            a_key.nsfw = j.GetBool();
	        break;
	    case discpp::AuditLogKey::APPLICATION_ID:
            a_key.application_id = discpp::Snowflake(j.GetString());
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
            a_key.channel_id = discpp::Snowflake(j.GetString());
	        break;
	    case discpp::AuditLogKey::INVITER_ID:
            a_key.inviter_id = discpp::Snowflake(j.GetString());
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
            a_key.id = discpp::Snowflake(j.GetString());
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

discpp::AuditLogChange::AuditLogChange(discpp::Client* client, rapidjson::Document& json) {
	key = json["key"].GetString();

	if (ContainsNotNull(json, "new_value")) {
	    std::unique_ptr<rapidjson::Document> new_value_json = GetDocumentInsideJson(json, "new_value");
		new_value = GetKey(client, key, *new_value_json);
	}

    if (ContainsNotNull(json, "old_value")) {
        std::unique_ptr<rapidjson::Document> old_value_json = GetDocumentInsideJson(json, "old_value");
        old_value = GetKey(client, key, *old_value_json);
    }
}

discpp::AuditEntryOptions::AuditEntryOptions(rapidjson::Document& json) {
	delete_member_days = GetDataSafely<std::string>(json, "delete_member_days");
	members_removed = GetDataSafely<std::string>(json, "members_removed");
	// @TODO: Make channel valid.
	if (ContainsNotNull(json, "channel_id")) {
        channel_id = discpp::Snowflake(json["channel_id"].GetString());
	}
    if (ContainsNotNull(json, "message_id")) {
        message_id = discpp::Snowflake(json["message_id"].GetString());
    }
	count = GetDataSafely<std::string>(json, "count");
	id = GetIDSafely(json, "id");
	type = GetDataSafely<std::string>(json, "type");
	role_name = GetDataSafely<std::string>(json, "role_name");
}

discpp::AuditLogEntry::AuditLogEntry(discpp::Client* client, rapidjson::Document& json) {
    target_id = GetDataSafely<std::string>(json, "target_id");
    if (ContainsNotNull(json, "changes")) {
        for (auto const& change : json["changes"].GetArray()) {
            rapidjson::Document change_json(rapidjson::kObjectType);
            change_json.CopyFrom(change, change_json.GetAllocator());

            changes.push_back(discpp::AuditLogChange(client, change_json));
        }
    }
    user = discpp::User(client, Snowflake(json["user_id"].GetString()));
    id = Snowflake(json["id"].GetString());
    action_type = static_cast<discpp::AuditLogEvent>(json["action_type"].GetInt());
    options = ConstructDiscppObjectFromJson(json, "options", discpp::AuditEntryOptions());
    reason = GetDataSafely<std::string>(json, "reason");
}

discpp::AuditLog::AuditLog(discpp::Client* client, rapidjson::Document& json) {
    for (auto const& webhook : json["webhooks"].GetArray()) {
        rapidjson::Document webhook_json(rapidjson::kObjectType);
        webhook_json.CopyFrom(webhook, webhook_json.GetAllocator());

        webhooks.push_back(discpp::Webhook(webhook_json));
    }

    for (auto const& user : json["user"].GetArray()) {
        rapidjson::Document user_json(rapidjson::kObjectType);
        user_json.CopyFrom(user, user_json.GetAllocator());

        users.push_back(discpp::User(client, user_json));
    }

    for (auto const& audit_log_entry : json["audit_log_entries"].GetArray()) {
        rapidjson::Document audit_log_entry_json(rapidjson::kObjectType);
        audit_log_entry_json.CopyFrom(audit_log_entry, audit_log_entry_json.GetAllocator());

        audit_log_entries.push_back(discpp::AuditLogEntry(client, audit_log_entry_json));
    }

    for (auto const& integration : json["integrations"].GetArray()) {
        rapidjson::Document integration_json(rapidjson::kObjectType);
        integration_json.CopyFrom(integration, integration_json.GetAllocator());

        integrations.emplace_back(nullptr, integration_json);
    }
}