#include "user.h"
#include "client.h"

namespace discpp {
	User::User(snowflake id) : discpp::DiscordObject(id) {
		/**
		 * @brief Constructs a discpp::User object from an id.
		 *
		 * This constructor searches the user cache to get a user object.
		 *
		 * ```cpp
		 *      discpp::User user(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the user.
		 *
		 * @return discpp::User, this is a constructor.
		 */

		auto it = discpp::globals::client_instance->members.find(id);
		if (it != discpp::globals::client_instance->members.end()) {
			*this = it->second->user;
		}
	}

	User::User(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::User object by parsing json.
		 *
		 * ```cpp
		 *      discpp::User user(json);
		 * ```
		 *
		 * @param[in] json The json that makes up of user object.
		 *
		 * @return discpp::User, this is a constructor.
		 */

		id = json["id"].GetString();
		username = GetDataSafely<std::string>(json, "username");
		discriminator = GetDataSafely<std::string>(json, "discriminator");
		avatar = GetDataSafely<std::string>(json, "avatar");
		bot = GetDataSafely<bool>(json, "bot");
		system = GetDataSafely<bool>(json, "system");
		mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
		locale = GetDataSafely<std::string>(json, "locale");
		verified = GetDataSafely<bool>(json, "verified");
		flags = GetDataSafely<int>(json, "flags");
		premium_type = static_cast<discpp::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
		public_flags = GetDataSafely<int>(json, "public_flags");
		created_at = FormatTimeFromSnowflake(id);
		mention = "<@" + id + ">";
	}

	Connection::Connection(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::Connection object by parsing json.
		 *
		 * ```cpp
		 *      discpp::Connection connection(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the connection object.
		 *
		 * @return discpp::Connection, this is a constructor.
		 */

		id = static_cast<snowflake>(json["id"].GetString());
		name = json["name"].GetString();
		type = json["type"].GetString();
		revoked = json["revoked"].GetBool();

		rapidjson::Value::ConstMemberIterator itr = json.FindMember("integrations");

		if (itr != json.MemberEnd()) {
			for (auto& integration : json["integrations"].GetArray()) {
				rapidjson::Document integration_json;
				integration_json.CopyFrom(integration, integration_json.GetAllocator());
				integrations.push_back(discpp::Integration(integration_json));
			}
		}
		verified = json["verified"].GetBool();
		friend_sync = json["friend_sync"].GetBool();
		show_activity = json["show_activity"].GetBool();
		
		itr = json.FindMember("visibility");
		if (itr != json.MemberEnd()){
			visibility = static_cast<ConnectionVisibility>(json["visibility"].GetInt());
		}
		else {
			visibility = ConnectionVisibility::NONE;
		}
	}

	discpp::Channel User::CreateDM() {
		/**
		 * @brief Create a DM channel with this user.
		 *
		 * ```cpp
		 *      discpp::Channel channel = user.CreateDM();
		 * ```
		 *
		 * @return discpp::Channel
		 */

		cpr::Body body("{\"recipient_id\": \"" + id + "\"}");
		rapidjson::Document result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Channel(result);
	}

	std::vector<Connection> User::GetUserConnections() {
		/**
		 * @brief Create all connections of this user.
		 *
		 * ```cpp
		 *      std::vector<discpp::Connection> conntections = user.GetUserConnections();
		 * ```
		 *
		 * @return std::vector<discpp::Connection>
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), id, RateLimitBucketType::GLOBAL);

		std::vector<Connection> connections;
		for (auto const& connection : result.GetArray()) {
			rapidjson::Document connection_json;
			connection_json.CopyFrom(connection, connection_json.GetAllocator());
			connections.push_back(discpp::Connection(connection_json));
		}

		return connections;
	}
	
	std::string User::GetAvatarURL(ImageType imgType) {
		/**
		 * @brief Retrieve user avatar url.
		 *
		 * ```cpp
		 *      std::string avatar_url = user.GetAvatarURL()
		 * ```
	     *
         * @param[in] imgType Optional parameter for type of image
		 *
		 * @return std::string
		 */

		std::string idString = this->id.c_str();
		if (this->avatar == "") {
			return cpr::Url("https://cdn.discppapp.com/embed/avatars/" + std::to_string(std::stoi(this->discriminator) % 5) + ".png");
		}
		else {
			std::string url = "https://cdn.discppapp.com/avatars/" + idString + "/" + this->avatar;
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
			default:
				return cpr::Url(url);
			}
		}
	}
}
