#include <iomanip>
#include "user.h"
#include "client.h"

namespace discpp {
	User::User(const snowflake& id) : discpp::DiscordObject(id) {
		auto it = discpp::globals::client_instance->members.find(id);
		if (it != discpp::globals::client_instance->members.end()) {
			*this = it->second->user;
		}
	}
	User::User(rapidjson::Document& json) {
		id = GetIDSafely(json, "id");
		username = GetDataSafely<std::string>(json, "username");
		discriminator = (unsigned short) strtoul(GetDataSafely<std::string>(json, "discriminator").c_str(), nullptr, 10);
		avatar = GetDataSafely<std::string>(json, "avatar");
		if (GetDataSafely<bool>(json, "bot")) flags |= 0b1;
        if (GetDataSafely<bool>(json, "system")) flags |= 0b10;
		public_flags = GetDataSafely<int>(json, "flags");
		premium_type = static_cast<discpp::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
		public_flags = GetDataSafely<int>(json, "public_flags");
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
		cpr::Body body("{\"recipient_id\": \"" + std::to_string(id) + "\"}");
		rapidjson::Document result = SendPostRequest(Endpoint("/users/@me/channels"), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Channel(result);
	}
	
	std::string User::GetAvatarURL(const ImageType& imgType) const {
		if (this->avatar == "") {
			return cpr::Url("https://cdn.discordapp.com/embed/avatars/" + std::to_string(this->discriminator % 5) + ".png");
		} else {
			std::string url = "https://cdn.discordapp.com/avatars/" + std::to_string(id) + "/" + this->avatar;
			ImageType tmp = imgType;
			if (tmp == ImageType::AUTO) tmp = StartsWith(this->avatar, "a_") ? ImageType::GIF : ImageType::PNG;
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
        return FormatTime(TimeFromSnowflake(id));
    }

    std::string User::CreateMention() {
        return "<@" + std::to_string(id) + ">";
    }

    bool User::IsBot() const {
	    return (flags & 0b1) == 0b1;
    }

    bool User::IsSystemUser() {
        return (flags & 0b10) == 0b10;
    }

    std::string User::GetDiscriminator() const {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(4) << discriminator;

        return stream.str();
    }
}
