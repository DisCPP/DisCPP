#include "user.h"
#include "client.h"
#include "guild.h"
#include "member.h"
#include "cache.h"
#include "exceptions.h"

#include <iomanip>
#include <sstream>

namespace discpp {
	User::User(discpp::Client* client, const Snowflake& id, bool can_request) : discpp::DiscordObject(client, id) {
        *this = client->cache->GetUser(id, can_request);
	}

	User::User(discpp::Client* client, rapidjson::Document& json) : discpp::DiscordObject(client) {
		id = GetIDSafely(json, "id");
		username = GetDataSafely<std::string>(json, "username");
		discriminator = (unsigned short) strtoul(GetDataSafely<std::string>(json, "discriminator").c_str(), nullptr, 10);
		if (ContainsNotNull(json, "avatar")) {
			std::string icon_str = json["avatar"].GetString();

			if (StartsWith(icon_str, "a_")) {
				is_avatar_gif = true;
				SplitAvatarHash(icon_str.substr(2), avatar_hex);
			} else {
				SplitAvatarHash(icon_str, avatar_hex);
			}
		}
		if (GetDataSafely<bool>(json, "bot")) flags |= 0b1;
        if (GetDataSafely<bool>(json, "system")) flags |= 0b10;
		//public_flags = GetDataSafely<int>(json, "public_flags");
	}

	User::Connection::Connection(rapidjson::Document& json) {

		id = json["id"].GetString();
		name = json["name"].GetString();
		type = json["type"].GetString();
		revoked = json["revoked"].GetBool();

		rapidjson::Value::ConstMemberIterator itr = json.FindMember("integrations");

		if (itr != json.MemberEnd()) {
			for (auto& integration : json["integrations"].GetArray()) {
				rapidjson::Document integration_json;
				integration_json.CopyFrom(integration, integration_json.GetAllocator());

				integrations.emplace_back(nullptr, integration_json);
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
		std::string body("{\"recipient_id\": \"" + std::to_string(id) + "\"}");
        discpp::Client* client = GetClient();
		std::unique_ptr<rapidjson::Document> result = SendPostRequest(client, Endpoint("/users/@me/channels"), DefaultHeaders(client, { {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Channel(client, *result);
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
			return "https://cdn.discordapp.com/embed/avatars/" + std::to_string(discriminator % 5) + ".png" + size;
		} else {
		    std::string avatar_str = CombineAvatarHash(avatar_hex);

			std::string url = "https://cdn.discordapp.com/avatars/" + std::to_string(id) + "/" + avatar_str;
			ImageType tmp = img_type;
			if (tmp == ImageType::AUTO) tmp = is_avatar_gif ? ImageType::GIF : ImageType::PNG;
			switch (tmp) {
			case ImageType::GIF:
				return url + ".gif" + size;
			case ImageType::JPEG:
				return url + ".jpeg" + size;
			case ImageType::PNG:
				return url + ".png" + size;
			case ImageType::WEBP:
				return url + ".webp" + size;
			default:
				return url + size;
			}
		}
	}

    std::string User::GetFormattedCreatedAt() const {
        return id.GetFormattedTimestamp();
    }

    std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>> User::GetMutualGuilds() {
         std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>> map;

         discpp::Client* client = GetClient();

         std::lock_guard<std::mutex> guilds_guard(client->cache->guilds_mutex);
         for (auto const& guild : client->cache->guilds) {
             try {
                 guild.second->GetMember(this->id);
                 map.emplace(guild.first, guild.second);
             } catch (const discpp::exceptions::DiscordObjectNotFound&) {

             }
         }

         return map;
    }

    std::chrono::system_clock::time_point User::GetCreatedAt() const {
        return std::chrono::system_clock::from_time_t(id.GetRawTime());
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
        discpp::Client* client = GetClient();
        if (client->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendPutRequest(client, Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL, "{\"type\":2}");
        }
    }

    void User::Unblock() {
        discpp::Client* client = GetClient();
        if(client->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(client, Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL);
        }
	}

    void User::AddFriend() {
        discpp::Client* client = GetClient();
        if (!client->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendPutRequest(client, Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL);
        }
    }

    void User::RemoveFriend() {
        discpp::Client* client = GetClient();
        if(client->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(client, Endpoint("users/@me/relationships/" + std::to_string(this->id)), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL);
        }
    }
}
