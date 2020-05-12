//
// Created by sean_ on 5/3/2020.
//

#include "audit_log.h"
#include "utils.h"
#include "user.h"
#include "guild.h"
#include "message.h"

// This is extremely ugly and probably slow, maybe theres a way we could trim this down?
discpp::AuditLogChangeKey GetKey(std::string key, nlohmann::json j) {
	discpp::AuditLogChangeKey a_key;

	if (key == "name") {
		a_key.name = j;
	} else if (key == "icon_hash") {
		a_key.icon_hash = j;
	} else if (key == "splash_hash") {
		a_key.splash_hash = j;
	} else if (key == "owner_id") {
		a_key.owner_id = j;
	} else if (key == "region") {
		a_key.region = j;
	} else if (key == "afk_channel_id") {
		a_key.afk_channel_id = j;
	} else if (key == "afk_timeout") {
		a_key.afk_timeout = j;
	} else if (key == "mfa_level") {
		a_key.mfa_level = j;
	} else if (key == "verification_level") {
		a_key.verification_level = j;
	} else if (key == "explicit_content_filter") {
		a_key.explicit_content_filter = j;
	} else if (key == "default_message_notifications") {
		a_key.default_message_notifications = j;
	} else if (key == "vanity_url_code") {
		a_key.vanity_url_code = j;
	} else if (key == "$add") {
		for (auto const& role : j) {
			a_key.roles_add.push_back(discpp::Role(role));
		}
	} else if (key == "$remove") {
		for (auto const& role : j) {
			a_key.roles_remove.push_back(discpp::Role(role));
		}
	} else if (key == "prune_delete_days") {
		a_key.prune_delete_days = j;
	} else if (key == "widget_enabled") {
		a_key.widget_enabled = j;
	} else if (key == "widget_channel_id") {
		a_key.widget_channel_id = j;
	} else if (key == "system_channel_id") {
		a_key.system_channel_id = j;
	} else if (key == "position") {
		a_key.position = j;
	} else if (key == "topic") {
		a_key.topic = j;
	} else if (key == "bitrate") {
		a_key.bitrate = j;
	} else if (key == "permission_overwrites") {
		for (auto const& perm : j) {
			a_key.permission_overwrites.push_back(discpp::Permissions(perm));
		}
	} else if (key == "nsfw") {
		a_key.nsfw = j;
	} else if (key == "application_id") {
		a_key.application_id = j;
	} else if (key == "rate_limit_per_user") {
		a_key.rate_limit_per_user = j;
	} else if (key == "permissions") {
		a_key.permissions = j;
	} else if (key == "color") {
		a_key.color = j;
	} else if (key == "hoist") {
		a_key.hoist = j;
	} else if (key == "mentionable") {
		a_key.mentionable = j;
	} else if (key == "allow") {
		a_key.allow = j;
	} else if (key == "deny") {
		a_key.deny = j;
	} else if (key == "code") {
		a_key.code = j;
	} else if (key == "channel_id") {
		a_key.channel_id = j;
	} else if (key == "inviter_id") {
		a_key.inviter_id = j;
	} else if (key == "max_uses") {
		a_key.max_uses = j;
	} else if (key == "uses") {
		a_key.uses = j;
	} else if (key == "max_age") {
		a_key.max_age = j;
	} else if (key == "temporary") {
		a_key.temporary = j;
	} else if (key == "deaf") {
		a_key.deaf = j;
	} else if (key == "mute") {
		a_key.mute = j;
	} else if (key == "nick") {
		a_key.nick = j;
	} else if (key == "avatar_hash") {
		a_key.avatar_hash = j;
	} else if (key == "id") {
		a_key.id = j;
	} else if (key == "type") {
		a_key.type = j;
	} else if (key == "enable_emoticons") {
		a_key.enable_emoticons = j;
	} else if (key == "expire_behavior") {
		a_key.expire_behavior = j;
	} else if (key == "expire_grace_period") {
		a_key.expire_grace_period = j;
	}

	return a_key;
}

discpp::AuditLogChange::AuditLogChange(nlohmann::json json) {
	key = json["key"];

	if (json.contains("new_value")) {
		new_value = GetKey(key, json["new_value"]);
	}

	if (json.contains("old_value")) {
		new_value = GetKey(key, json["old_value"]);
	}
}

discpp::AuditEntryOptions::AuditEntryOptions(nlohmann::json json) {
	delete_member_days = GetDataSafely<std::string>(json, "delete_member_days");
	members_removed = GetDataSafely<std::string>(json, "members_removed");
	channel = (json.contains("channel_id")) ? discpp::Channel(json["channel_id"].get<snowflake>()) : discpp::Channel();
	message = (json.contains("message_id")) ? new discpp::Message(json["message_id"].get<snowflake>()) : new discpp::Message();
	count = GetDataSafely<std::string>(json, "count");
	id = GetDataSafely<discpp::snowflake>(json, "id");
	type = GetDataSafely<std::string>(json, "type");
	role_name = GetDataSafely<std::string>(json, "role_name");
}

discpp::AuditLogEntry::AuditLogEntry(nlohmann::json json) {
    target_id = GetDataSafely<std::string>(json, "target_id");
    if (json.contains("changes")) {
        for (auto const& change : json["changes"]) {
            changes.push_back(discpp::AuditLogChange(change));
        }
    }
    user = discpp::User(json["user_id"].get<snowflake>());
    id = json["id"];
    action_type = static_cast<discpp::AuditLogEvent>(json["action_type"].get<int>());
    options = (json.contains("options")) ? AuditEntryOptions(json["options"]) : AuditEntryOptions();
    reason = GetDataSafely<std::string>(json, "reason");
}

discpp::AuditLog::AuditLog(nlohmann::json json) {
    for (auto const& webhook : json["webhooks"]) {
        webhooks.push_back(discpp::Webhook(webhook));
    }

    for (auto const& user : json["user"]) {
        users.push_back(discpp::User(user));
    }

    for (auto const& audit_log_entry : json["audit_log_entries"]) {
        audit_log_entries.push_back(discpp::AuditLogEntry(audit_log_entry));
    }

    for (auto const& integration : json["integrations"]) {
        integrations.push_back(discpp::Integration(integration));
    }
}