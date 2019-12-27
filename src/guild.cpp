#include "guild.h"
#include "member.h"
#include "channel.h"

namespace discord {
	Guild::Guild(snowflake id) {

	}

	Guild::Guild(nlohmann::json json) {
		id = ToSnowflake(json["id"]);
		name = json["name"];
		icon = json["icon"];
		splash = GetDataSafely<std::string>(json, "splash");
		owner = GetDataSafely<bool>(json, owner);
		owner_id = ToSnowflake(json["owner_id"]);
		permissions = GetDataSafely<int>(json, "permissions");
		region = json["region"];
		afk_channel_id = GetSnowflakeSafely(json, "afk_channel_id");
		afk_timeout = json["afk_timeout"].get<int>();
		embed_enabled = GetDataSafely<bool>(json, "embed_enabled");
		embed_channel_id = GetSnowflakeSafely(json, "embed_channel_id");
		verification_level = static_cast<discord::specials::VerificationLevel>(json["verification_level"].get<int>());
		default_message_notifications = static_cast<discord::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].get<int>());
		explicit_content_filter = static_cast<discord::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].get<int>());
		// roles
		for (auto& emoji : json["emojis"]) {
			emojis.push_back(discord::Emoji(emoji));
		}
		// features
		mfa_level = static_cast<discord::specials::MFALevel>(json["mfa_level"].get<int>());
		application_id = GetSnowflakeSafely(json, "application_id");
		widget_enabled = GetDataSafely<bool>(json, "widget_enabled");
		widget_channel_id = GetSnowflakeSafely(json, "widget_channel_id");
		system_channel_id = ToSnowflake(json["system_channel_id"]);
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		large = GetDataSafely<bool>(json, "large");
		unavailable = GetDataSafely<bool>(json, "unavailable");
		member_count = GetDataSafely<int>(json, "member_count");
		// voice_states
		for (auto& member : json["members"]) {
			members.push_back(discord::Member(member));
		}
		for (auto& channel : json["channels"]) {
			channels.push_back(discord::Channel(channel));
		}
		// presences
		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");
		vanity_url_code = GetDataSafely<std::string>(json, "vanity_url_code");
		description = GetDataSafely<std::string>(json, "description");
		banner = GetDataSafely<std::string>(json, "banner");
		premium_tier = static_cast<discord::specials::NitroTier>(json["premium_tier"].get<int>());
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = json["preferred_locale"];
	}
}