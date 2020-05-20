//
// Created by SeanOMik on 5/3/2020.
//

#include "audit_log.h"
#include "utils.h"
#include "user.h"
#include "guild.h"
#include "message.h"
#include "client.h"

// This is extremely ugly and probably slow, maybe theres a way we could trim this down?
discpp::AuditLogChangeKey GetKey(std::string key, rapidjson::Document& j) {
	discpp::AuditLogChangeKey a_key;

	if (key == "name") {
		a_key.name = j.GetString();
	} else if (key == "icon_hash") {
		a_key.icon_hash = j.GetString();
	} else if (key == "splash_hash") {
		a_key.splash_hash = j.GetString();
	} else if (key == "owner_id") {
		a_key.owner_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "region") {
		a_key.region = j.GetString();
	} else if (key == "afk_channel_id") {
		a_key.afk_channel_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "afk_timeout") {
		a_key.afk_timeout = j.GetInt();
	} else if (key == "mfa_level") {
		a_key.mfa_level = j.GetInt();
	} else if (key == "verification_level") {
		a_key.verification_level = j.GetInt();
	} else if (key == "explicit_content_filter") {
		a_key.explicit_content_filter = j.GetInt();
	} else if (key == "default_message_notifications") {
		a_key.default_message_notifications = j.GetInt();
	} else if (key == "vanity_url_code") {
		a_key.vanity_url_code = j.GetString();
	} else if (key == "$add") {
		for (auto const& role : j.GetArray()) {
		    rapidjson::Document role_json(rapidjson::kObjectType);
		    role_json.CopyFrom(role, role_json.GetAllocator());

			a_key.roles_add.push_back(discpp::Role(role_json));
		}
	} else if (key == "$remove") {
		for (auto const& role : j.GetArray()) {
            rapidjson::Document role_json(rapidjson::kObjectType);
            role_json.CopyFrom(role, role_json.GetAllocator());

			a_key.roles_remove.push_back(discpp::Role(role_json));
		}
	} else if (key == "prune_delete_days") {
		a_key.prune_delete_days = j.GetInt();
	} else if (key == "widget_enabled") {
		a_key.widget_enabled = j.GetBool();
	} else if (key == "widget_channel_id") {
		a_key.widget_channel_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "system_channel_id") {
		a_key.system_channel_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "position") {
		a_key.position = j.GetInt();
	} else if (key == "topic") {
		a_key.topic = j.GetString();
	} else if (key == "bitrate") {
		a_key.bitrate = j.GetInt();
	} else if (key == "permission_overwrites") {
		for (auto const& perm : j.GetArray()) {
            rapidjson::Document perm_json(rapidjson::kObjectType);
            perm_json.CopyFrom(perm, perm_json.GetAllocator());

			a_key.permission_overwrites.push_back(discpp::Permissions(perm_json));
		}
	} else if (key == "nsfw") {
		a_key.nsfw = j.GetBool();
	} else if (key == "application_id") {
		a_key.application_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "rate_limit_per_user") {
		a_key.rate_limit_per_user = j.GetInt();
	} else if (key == "permissions") {
		a_key.permissions = j.GetInt();
	} else if (key == "color") {
		a_key.color = j.GetInt();
	} else if (key == "hoist") {
		a_key.hoist = j.GetBool();
	} else if (key == "mentionable") {
		a_key.mentionable = j.GetBool();
	} else if (key == "allow") {
		a_key.allow = j.GetBool();
	} else if (key == "deny") {
		a_key.deny = j.GetBool();
	} else if (key == "code") {
		a_key.code = j.GetString();
	} else if (key == "channel_id") {
		a_key.channel_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "inviter_id") {
		a_key.inviter_id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "max_uses") {
		a_key.max_uses = j.GetInt();
	} else if (key == "uses") {
		a_key.uses = j.GetInt();
	} else if (key == "max_age") {
		a_key.max_age = j.GetInt();
	} else if (key == "temporary") {
		a_key.temporary = j.GetBool();
	} else if (key == "deaf") {
		a_key.deaf = j.GetBool();
	} else if (key == "mute") {
		a_key.mute = j.GetBool();
	} else if (key == "nick") {
		a_key.nick = j.GetString();
	} else if (key == "avatar_hash") {
		a_key.avatar_hash = j.GetString();
	} else if (key == "id") {
		a_key.id = discpp::SnowflakeFromString(j.GetString());
	} else if (key == "type") {
		a_key.type = j.GetString();
	} else if (key == "enable_emoticons") {
		a_key.enable_emoticons = j.GetBool();
	} else if (key == "expire_behavior") {
		a_key.expire_behavior = j.GetInt();
	} else if (key == "expire_grace_period") {
		a_key.expire_grace_period = j.GetInt();
	}

	return a_key;
}

discpp::AuditLogChange::AuditLogChange(rapidjson::Document& json) {
	key = json["key"].GetString();

	if (ContainsNotNull(json, "new_value")) {
	    rapidjson::Document new_value_json = GetDocumentInsideJson(json, "new_value");
		new_value = GetKey(key, new_value_json);
	}

    if (ContainsNotNull(json, "old_value")) {
        rapidjson::Document new_value_json = GetDocumentInsideJson(json, "old_value");
        old_value = GetKey(key, new_value_json);
    }
}

discpp::AuditEntryOptions::AuditEntryOptions(rapidjson::Document& json) {
	delete_member_days = GetDataSafely<std::string>(json, "delete_member_days");
	members_removed = GetDataSafely<std::string>(json, "members_removed");
	// @TODO: Make channel valid.
	if (ContainsNotNull(json, "channel_id")) {
	    channel = std::make_shared<discpp::Channel>(globals::client_instance->GetChannel(discpp::SnowflakeFromString(json["channel_id"].GetString())));
	} else channel = nullptr;
    message = std::make_shared<discpp::Message>(ConstructDiscppObjectFromID(json, "message_id", discpp::Message()));
	count = GetDataSafely<std::string>(json, "count");
	id = GetIDSafely(json, "id");
	type = GetDataSafely<std::string>(json, "type");
	role_name = GetDataSafely<std::string>(json, "role_name");
}

discpp::AuditLogEntry::AuditLogEntry(rapidjson::Document& json) {
    target_id = GetDataSafely<std::string>(json, "target_id");
    if (ContainsNotNull(json, "changes")) {
        for (auto const& change : json["changes"].GetArray()) {
            rapidjson::Document change_json(rapidjson::kObjectType);
            change_json.CopyFrom(change, change_json.GetAllocator());

            changes.push_back(discpp::AuditLogChange(change_json));
        }
    }
    user = discpp::User(SnowflakeFromString(json["user_id"].GetString()));
    id = SnowflakeFromString(json["id"].GetString());
    action_type = static_cast<discpp::AuditLogEvent>(json["action_type"].GetInt());
    options = ConstructDiscppObjectFromJson(json, "options", discpp::AuditEntryOptions());
    reason = GetDataSafely<std::string>(json, "reason");
}

discpp::AuditLog::AuditLog(rapidjson::Document& json) {
    for (auto const& webhook : json["webhooks"].GetArray()) {
        rapidjson::Document webhook_json(rapidjson::kObjectType);
        webhook_json.CopyFrom(webhook, webhook_json.GetAllocator());

        webhooks.push_back(discpp::Webhook(webhook_json));
    }

    for (auto const& user : json["user"].GetArray()) {
        rapidjson::Document user_json(rapidjson::kObjectType);
        user_json.CopyFrom(user, user_json.GetAllocator());

        users.push_back(discpp::User(user_json));
    }

    for (auto const& audit_log_entry : json["audit_log_entries"].GetArray()) {
        rapidjson::Document audit_log_entry_json(rapidjson::kObjectType);
        audit_log_entry_json.CopyFrom(audit_log_entry, audit_log_entry_json.GetAllocator());

        audit_log_entries.push_back(discpp::AuditLogEntry(audit_log_entry_json));
    }

    for (auto const& integration : json["integrations"].GetArray()) {
        rapidjson::Document integration_json(rapidjson::kObjectType);
        integration_json.CopyFrom(integration, integration_json.GetAllocator());

        integrations.push_back(discpp::Integration(integration_json));
    }
}