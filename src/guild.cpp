#include "guild.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "emoji.h"
#include "bot.h"

namespace discord {
	Guild::Guild(snowflake id) {
		auto guild = std::find_if(discord::globals::bot_instance->guilds.begin(), discord::globals::bot_instance->guilds.end(), [id](discord::Guild a) { return id == a.id; });

		if (guild != discord::globals::bot_instance->guilds.end()) {
			*this = *guild;
		}
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
		for (auto& role : json["roles"]) {
			roles.push_back(discord::Role(role));
		}
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

	void Guild::DeleteGuild() {
		SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id)), DefaultHeaders());
	}

	std::vector<discord::Channel> Guild::GetChannels() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/channels"), DefaultHeaders(), {}, {});
		std::vector<discord::Channel> channels;
		for (auto& channel : result) {
			channels.push_back(discord::Channel(channel));
		}
		return channels;
	}

	discord::Member Guild::GetMember(snowflake id) {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders(), {}, {});
		return discord::Member(result);
	}

	std::vector<discord::Member> Guild::GetMembers(int limit) {
		cpr::Body body("{\"limit\": " + std::to_string(limit) + "}");
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/members"), DefaultHeaders(), {}, body);

		std::vector<discord::Member> members;
		for (auto& member : result) {
			members.push_back(discord::Member(member));
		}
		return members;
	}

	std::vector<discord::Member> Guild::GetMembers(snowflake id) {
		cpr::Body body("{\"after\": " + std::to_string(id) + "}");
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/members"), DefaultHeaders(), {}, body);

		std::vector<discord::Member> members;
		for (auto& member : result) {
			members.push_back(discord::Member(member));
		}
		return members;
	}

	discord::Member Guild::AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf) {
		std::string json_roles = "[";
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + std::to_string(role.id) + "\"";
			} else {
				json_roles += ", \"" + std::to_string(role.id) + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");
		nlohmann::json result = SendPutRequest(Endpoint("/guilds/" + std::to_string(this->id) + "/members/" + std::to_string(id)), DefaultHeaders(), body);
		return (result == "{}") ? discord::Member(id) : discord::Member(result);
	}

	std::vector<discord::Emoji> Guild::GetEmojis() {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis"), DefaultHeaders(), {}, {});

		std::vector<discord::Emoji> emojis;
		for (auto& emoji : result) {
			emojis.push_back(discord::Emoji(emoji));
		}
		return emojis;
	}

	discord::Emoji Guild::GetEmoji(snowflake id) {
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis/" + std::to_string(id)), DefaultHeaders(), {}, {});
		return discord::Emoji(result);
	}

	discord::Emoji Guild::ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles) {
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis/" + std::to_string(emoji.id)), DefaultHeaders(), {});
		return emoji;
	}

	void Guild::DeleteEmoji(discord::Emoji emoji) {
		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/" + std::to_string(id) + "/emojis/" + std::to_string(emoji.id)), DefaultHeaders());
	}
}