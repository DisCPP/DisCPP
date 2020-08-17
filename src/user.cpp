#include "user.h"
#include "client.h"
#include "guild.h"
#include "member.h"

#include <iomanip>
#include <discpp/exceptions.h>

namespace discpp {
	User::User(const Snowflake& id) : discpp::DiscordObject(id) {
		auto it = discpp::globals::client_instance->cache.members.find(id);
		if (it != discpp::globals::client_instance->cache.members.end()) {
			*this = it->second->user;
		}
	}

	User::User(const discpp::JsonObject& json) {
		id = json.GetIDSafely("id");
		username = json.Get<std::string>("username");
		discriminator = (unsigned short) strtoul(json.Get<std::string>("discriminator").c_str(), nullptr, 10);
		if (json.ContainsNotNull("avatar")) {
			auto icon_str = json.Get<std::string>("avatar");

			if (StartsWith(icon_str, "a_")) {
				is_avatar_gif = true;
				SplitAvatarHash(icon_str.substr(2), avatar_hex);
			} else {
				SplitAvatarHash(icon_str, avatar_hex);
			}
		}
		if (json.Get<bool>("bot")) flags |= 0b1;
        if (json.Get<bool>("system")) flags |= 0b10;
		//public_flags = json.Get<int>("public_flags");
	}

	User::Connection::Connection(const discpp::JsonObject& json) {

		id = json["id"].GetString();
        name = json["name"].GetString();
        type = json["type"].GetString();
        revoked = json["revoked"].GetBool();

		if (json.ContainsNotNull("integrations")) {
		    json["integrations"].IterateThrough([&](const discpp::JsonObject& json)->bool {
                integrations.emplace_back(json);
                return true;
		    });
		}
		verified = json["verified"].GetBool();
		friend_sync = json["friend_sync"].GetBool();
		show_activity = json["show_activity"].GetBool();

        if (json.ContainsNotNull("visibility")) {
			visibility = static_cast<ConnectionVisibility>(json["visibility"].GetInt());
		} else {
			visibility = ConnectionVisibility::NONE;
		}
	}

	discpp::Channel User::CreateDM() {
		cpr::Body body("{\"recipient_id\": \"" + std::to_string(id) + "\"}");
		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Channel(*result);
	}
	
	std::string User::GetAvatarURL(const ImageType& img_type, const ImageSize img_size) const {
	    std::string size = "?size=";

	    switch (img_size) {
	        case ImageSize::x128:
	            size += "128";
	            break;
	        case ImageSize::x256:
	            size += "256";
	            break;
	        case ImageSize::x512:
	            size += "512";
	            break;
	        case ImageSize::x1024:
	            size += "1024";
	            break;
	        default:
	            break;
	    }

        if (avatar_hex[0] == 0) {
			return cpr::Url("https://cdn.discordapp.com/embed/avatars/" + std::to_string(discriminator % 5) + ".png" + size);
		} else {
		    std::string avatar_str = CombineAvatarHash(avatar_hex);

			std::string url = "https://cdn.discordapp.com/avatars/" + std::to_string(id) + "/" + avatar_str;
			ImageType tmp = img_type;
			if (tmp == ImageType::AUTO) tmp = is_avatar_gif ? ImageType::GIF : ImageType::PNG;
			switch (tmp) {
			case ImageType::GIF:
				return cpr::Url(url + ".gif" + size);
			case ImageType::JPEG:
				return cpr::Url(url + ".jpeg" + size);
			case ImageType::PNG:
				return cpr::Url(url + ".png" + size);
			case ImageType::WEBP:
				return cpr::Url(url + ".webp" + size);
			default:
				return cpr::Url(url + size);
			}
		}
	}

    std::string User::GetFormattedCreatedAt() const {
        return FormatTime(TimeFromSnowflake(id));
    }

    std::chrono::system_clock::time_point User::GetCreatedAt() const {
        return std::chrono::system_clock::from_time_t(TimeFromSnowflake(id));
	}

    std::string User::CreateMention() {
        return "<@" + std::to_string(id) + ">";
    }

    bool User::IsBot() const {
	    return flags & 1;
    }

    bool User::IsSystemUser() {
        return flags & 2;
    }

    std::string User::GetDiscriminator() const {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(4) << discriminator;

        return stream.str();
    }

    void User::Block() {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendPutRequest(Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL, "{\"type\":2}");
        }
    }

    void User::Unblock() {
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
	}

    void User::AddFriend() {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendPutRequest(Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }

    void User::RemoveFriend() {
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }
}
