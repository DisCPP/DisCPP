#include "user.h"
#include "bot.h"
#include "member.h"

namespace discord {
	User::User(snowflake id) : discord::DiscordObject(id) {
		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			*this = member->user;
		}
	}

	User::User(nlohmann::json json) {
		id = json["id"].get<snowflake>();
		username = json["username"];
		discriminator = json["discriminator"];
		avatar = GetDataSafely<std::string>(json, "avatar");
		bot = GetDataSafely<bool>(json, "bot");
		system = GetDataSafely<bool>(json, "system");
		mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
		locale = GetDataSafely<std::string>(json, "locale");
		verified = GetDataSafely<bool>(json, "verified");
		flags = GetDataSafely<int>(json, "flags");
		premium_type = static_cast<discord::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
	}

	Connection::Connection(nlohmann::json json) {
		id = json["id"];
		name = json["name"];
		type = json["tpye"];
		revoked = json["revoked"].get<bool>();
		for (auto& integration : json["integrations"]) {
			integrations.push_back(discord::GuildIntegration(integration));
		}
		verified = json["verified"].get<bool>();
		friend_sync = json["friend_sync"].get<bool>();
		show_activity = json["show_activity"].get<bool>();
		visibility = static_cast<ConnectionVisibility>(GetDataSafely<int>(json, "visibility"));
	}

	discord::Channel User::CreateDM() {
		cpr::Body body(Format("{\"recipient_id\": \"%\"}", id));
		nlohmann::json result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL, body);
		return discord::Channel(result);
	}

	std::vector<Connection> User::GetUserConnections() {
		nlohmann::json result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), id, RateLimitBucketType::GLOBAL);

		std::vector<Connection> connections;
		for (auto& connection : result) {
			connections.push_back(discord::Connection(connection));
		}
		return connections;
	}

}