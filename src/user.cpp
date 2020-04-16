#include "user.h"
#include "bot.h"
#include "member.h"
#include "utils.h"
#include "cpr/cpr.h"

namespace discord {
	User::User(snowflake id) : discord::DiscordObject(id) {
		/**
		 * @brief Constructs a discord::User object from an id.
		 *
		 * This constructor searches the user cache to get a user object.
		 *
		 * ```cpp
		 *      discord::User user(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the user.
		 *
		 * @return discord::User, this is a constructor.
		 */

		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			*this = member->user;
		}
	}

	User::User(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::User object by parsing json.
		 *
		 * ```cpp
		 *      discord::User user(json);
		 * ```
		 *
		 * @param[in] json The json that makes up of user object.
		 *
		 * @return discord::User, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		username = GetDataSafely<std::string>(json, "username");
		discriminator = GetDataSafely<std::string>(json, "discriminator");
		avatar = GetDataSafely<std::string>(json, "avatar");
		bot = GetDataSafely<bool>(json, "bot");
		system = GetDataSafely<bool>(json, "system");
		mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
		locale = GetDataSafely<std::string>(json, "locale");
		verified = GetDataSafely<bool>(json, "verified");
		flags = GetDataSafely<int>(json, "flags");
		premium_type = (json.contains("premium_type")) ? static_cast<discord::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type")) : discord::specials::NitroSubscription::NO_NITRO;
		created_at = FormatTimeFromSnowflake(id);
	}

	Connection::Connection(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Connection object by parsing json.
		 *
		 * ```cpp
		 *      discord::Connection connection(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the connection object.
		 *
		 * @return discord::Connection, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		name = GetDataSafely<std::string>(json, "name");
		type = GetDataSafely<std::string>(json, "type");
		revoked = GetDataSafely<bool>(json, "revoked");
		if (json.contains("integrations")) {
			for (auto& integration : json["integrations"]) {
				integrations.push_back(discord::GuildIntegration(integration));
			}
		}
		verified = GetDataSafely<bool>(json, "verified");
		friend_sync = GetDataSafely<bool>(json, "friend_sync");
		show_activity = GetDataSafely<bool>(json, "show_activity");
		visibility = (json.contains("visibility")) ? static_cast<ConnectionVisibility>(GetDataSafely<int>(json, "visibility")) : ConnectionVisibility::NONE;
	}

	discord::Channel User::CreateDM() {
		/**
		 * @brief Create a DM channel with this user.
		 *
		 * ```cpp
		 *      discord::Channel channel = user.CreateDM();
		 * ```
		 *
		 * @return discord::Channel
		 */

		cpr::Body body("{\"recipient_id\": \"" + id + "\"}");
		nlohmann::json result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL, body);
		return discord::Channel(result);
	}

	std::vector<Connection> User::GetUserConnections() {
		/**
		 * @brief Create all connections of this user.
		 *
		 * ```cpp
		 *      std::vector<discord::Connection> conntections = user.GetUserConnections();
		 * ```
		 *
		 * @return std::vector<discord::Connection>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), id, RateLimitBucketType::GLOBAL);

		std::vector<Connection> connections;
		for (auto& connection : result) {
			connections.push_back(discord::Connection(connection));
		}
		return connections;
	}
	
	std::string User::GetAvatarURL(ImageType imgType) {
		std::string idString = this->id.c_str();
		if (this->avatar == "") {
			return cpr::Url("https://cdn.discordapp.com/embed/avatars/" + std::to_string(std::stoi(this->discriminator) % 5) + ".png");
		}
		else {
			std::string url = "https://cdn.discordapp.com/avatars/" + idString + "/" + this->avatar;
			if (imgType == ImageType::AUTO) imgType = StartsWith(this->avatar, "a_") ? ImageType::GIF : ImageType::PNG;
			switch (imgType) {
			case ImageType::GIF:
				return cpr::Url(url + ".gif");
			case ImageType::JPEG:
				return cpr::Url(url + ".jpeg");
			case ImageType::PNG:
				return cpr::Url(url + ".png");
			case ImageType::WEBP:
				return cpr::Url(url + ".webp");
			}
		}
	}
}
