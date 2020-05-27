#include <iomanip>
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

		id = GetIDSafely(json, "id");
		username = GetDataSafely<std::string>(json, "username");
		discriminator = (unsigned short) strtoul(GetDataSafely<std::string>(json, "discriminator").c_str(), nullptr, 10);
		avatar = GetDataSafely<std::string>(json, "avatar");
		if (GetDataSafely<bool>(json, "bot")) flags |= 0b1;
        if (GetDataSafely<bool>(json, "system")) flags |= 0b10;
		flags = GetDataSafely<int>(json, "flags");
		premium_type = static_cast<discpp::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
		public_flags = GetDataSafely<int>(json, "public_flags");
	}

	User::Connection::Connection(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::User::Connection object by parsing json.
		 *
		 * ```cpp
		 *      discpp::User::Connection connection(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the connection object.
		 *
		 * @return discpp::User::Connection, this is a constructor.
		 */

		id = json["id"].GetString();
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
		} else {
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

		cpr::Body body("{\"recipient_id\": \"" + std::to_string(id) + "\"}");
		rapidjson::Document result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Channel(result);
	}
	
	std::string User::GetAvatarURL(ImageType imgType) const {
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

		if (this->avatar == "") {
			return cpr::Url("https://cdn.discordapp.com/embed/avatars/" + std::to_string(this->discriminator % 5) + ".png");
		} else {
			std::string url = "https://cdn.discordapp.com/avatars/" + std::to_string(id) + "/" + this->avatar;
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

    std::string User::CreatedAt() {
        /**
         * @brief Gets the created at time and date for this user.
         *
         * ```cpp
         *      std::string created_at = user.CreatedAt();
         * ```
         *
         * @return std::string
         */
        return FormatTimeFromSnowflake(id);
    }

    std::string User::CreateMention() {
        /**
         * @brief Creates a mention string for this user.
         *
         * ```cpp
         *      std::string mention = user.CreateMention();
         * ```
         *
         * @return std::string
         */
        return "<@" + std::to_string(id) + ">";
    }

    bool User::IsBot() {
        /**
         * @brief Checks if the user is a bot
         *
         * @return bool
         */
	    return (flags & 0b1) == 0b1;
    }

    bool User::IsSystemUser() {
        /**
         * @brief Checks if the user is a discord staff user.
         *
         * @return bool
         */
        return (flags & 0b10) == 0b10;
    }

    std::string User::GetDiscriminator() const {
        /**
         * @brief Gets the users discriminator as a string.
         *
         * @return std::string
         */
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(4) << discriminator;

        return stream.str();
    }
}
