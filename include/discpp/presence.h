#ifndef DISCPP_ACTIVITY_H
#define DISCPP_ACTIVITY_H

#include <string>
#include <ctime>
#include <chrono>

#include "snowflake.h"
#include "utils.h"
#include "emoji.h"

namespace discpp {

    class Activity {
    public:
        enum ActivityType : int {
            NONE = -1,
            GAME = 0,
            STREAMING = 1,
            LISTENING = 2,
            CUSTOM = 4 // Not supported for bots yet. So that makes me sad :(
        };

        struct Party {
            Party() = default;
            Party(rapidjson::Document& json) {
                if (ContainsNotNull(json, "id")) {
                    id = json["id"].GetString();
                }
                if (ContainsNotNull(json, "size")) {
                    current_size = json["size"].GetArray()[0].GetInt();
                    max_size = json["size"].GetArray()[1].GetInt();
                }
            }

            std::unique_ptr<rapidjson::Document> ToJson() const {
                auto json = std::make_unique<rapidjson::Document>(rapidjson::kObjectType);
                json->AddMember("id", id, json->GetAllocator());
                json->AddMember("current_size", current_size, json->GetAllocator());
                json->AddMember("max_size", max_size, json->GetAllocator());
                
                return std::move(json);
            }

            std::string id;
            int current_size;
            int max_size;
        };

        struct Assets {
            Assets() = default;
            Assets(rapidjson::Document& json) {
                if (ContainsNotNull(json, "large_image")) {
                    large_image = json["large_image"].GetString();
                }
                if (ContainsNotNull(json, "large_text")) {
                    large_text = json["large_text"].GetString();
                }
                if (ContainsNotNull(json, "small_image")) {
                    small_image = json["small_image"].GetString();
                }
                if (ContainsNotNull(json, "small_text")) {
                    small_text = json["small_text"].GetString();
                }
            }

            std::unique_ptr<rapidjson::Document> ToJson() const {
                auto json = std::make_unique<rapidjson::Document>(rapidjson::kObjectType);
                json->AddMember("large_image", large_image, json->GetAllocator());
                json->AddMember("large_text", large_text, json->GetAllocator());
                json->AddMember("small_image", small_image, json->GetAllocator());
                json->AddMember("small_text", small_text, json->GetAllocator());
                
                return std::move(json);
            }

            std::string large_image;
            std::string large_text;
            std::string small_image;
            std::string small_text;
        };

        struct Secrets {
            Secrets() = default;
            Secrets(rapidjson::Document& json) {
                if (ContainsNotNull(json, "join")) {
                    join = json["join"].GetString();
                }
                if (ContainsNotNull(json, "spectate")) {
                    spectate = json["spectate"].GetString();
                }
                if (ContainsNotNull(json, "match")) {
                    match = json["match"].GetString();
                }
            }

            std::unique_ptr<rapidjson::Document> ToJson() const {
                auto json = std::make_unique<rapidjson::Document>(rapidjson::kObjectType);
                json->AddMember("join", join, json->GetAllocator());
                json->AddMember("spectate", spectate, json->GetAllocator());
                json->AddMember("match", match, json->GetAllocator());

                return std::move(json);
            }

            std::string join;
            std::string spectate;
            std::string match;
        };

        Activity() = default;
        Activity(discpp::Client* client, rapidjson::Document& json) {
            name = json["name"].GetString();
            type = static_cast<ActivityType>(json["type"].GetInt());
            if (ContainsNotNull(json, "url")) {
                url = json["url"].GetString();
            }
            created_at = std::chrono::system_clock::from_time_t(json["created_at"].Get<std::time_t>());
            if (ContainsNotNull(json, "timestamps")) {
                rapidjson::Document timestamps_json(rapidjson::kObjectType);
                timestamps_json.CopyFrom(json["timestamps"], timestamps_json.GetAllocator());

                if (ContainsNotNull(timestamps_json, "start")) {
                    this->timestamps.emplace("start", std::chrono::system_clock::from_time_t(json["start"].Get<std::time_t>()));
                }

                if (ContainsNotNull(timestamps_json, "end")) {
                    this->timestamps.emplace("end", std::chrono::system_clock::from_time_t(json["end"].Get<std::time_t>()));
                }
            }
            application_id = json["id"].GetString();
            if (ContainsNotNull(json, "details")) {
                details = json["details"].GetString();
            }
            if (ContainsNotNull(json, "state")) {
                state = json["state"].GetString();
            }
            emoji = ConstructDiscppObjectFromJson(client, json, "emoji", discpp::Emoji());

            if (ContainsNotNull(json, "party")) {
                rapidjson::Document party_json;
                party_json.CopyFrom(json["party"], json.GetAllocator());
                
                party = std::make_shared<discpp::Activity::Party>(party_json);
            }

            if (ContainsNotNull(json, "assets")) {
                rapidjson::Document assets_json;
                assets_json.CopyFrom(json["assets"], json.GetAllocator());
                
                assets = std::make_shared<discpp::Activity::Assets>(assets_json);
            }

            if (ContainsNotNull(json, "secrets")) {
                rapidjson::Document secrets_json;
                secrets_json.CopyFrom(json["secrets"], json.GetAllocator());
                
                secrets = std::make_shared<discpp::Activity::Secrets>(secrets_json);
            }

            instance = GetDataSafely<bool>(json, "instance");
            flags = GetDataSafely<int>(json, "flags");
        }

        rapidjson::Document ToJson(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) const {
            rapidjson::Document json(rapidjson::kObjectType);
            json.AddMember("name", name, allocator);
            json.AddMember("type", (int) type, allocator);
            if (!url.empty()) {
                json.AddMember("url", url, allocator);
            }
            if (std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count() != 0) {
                json.AddMember("created_at", std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count(), allocator);
            } else {
                throw std::runtime_error("created_at must be set somehow!");
            }

            auto start_it = timestamps.find("start");
            if (start_it != timestamps.end()) {
                json.AddMember("start", std::chrono::duration_cast<std::chrono::seconds>(start_it->second.time_since_epoch()).count(), allocator);
            }

            auto end_it = timestamps.find("end");
            if (end_it != timestamps.end()) {
                json.AddMember("end", std::chrono::duration_cast<std::chrono::seconds>(end_it->second.time_since_epoch()).count(), allocator);
            }

            if (!application_id.empty()) {
                json.AddMember("application_id", application_id, allocator);
            }

            if (!details.empty()) {
                json.AddMember("details", details, allocator);
            }

            if (!state.empty()) {
                json.AddMember("state", state, allocator);
            }

            // Create an Activity Emoji
            if (!emoji.name.empty() || emoji.id != 0) {
                rapidjson::Document activity_emoji(rapidjson::kObjectType);
                activity_emoji.AddMember("name", emoji.name, allocator);
                activity_emoji.AddMember("id", emoji.id, allocator);
                activity_emoji.AddMember("animated", emoji.animated, allocator);
                json.AddMember("emoji", activity_emoji, allocator);
            }

            // Add party
            if (party) {
                rapidjson::Document party_json(rapidjson::kObjectType);
                party_json.AddMember("id", party->id, allocator);
                party_json.AddMember("current_size", party->current_size, allocator);
                party_json.AddMember("max_size", party->max_size, allocator);
                json.AddMember("party", party_json, allocator);
            }

            // Add assets
            if (assets) {
                rapidjson::Document assets_json(rapidjson::kObjectType);
                assets_json.AddMember("large_image", assets->large_image, allocator);
                assets_json.AddMember("large_text", assets->large_text, allocator);
                assets_json.AddMember("small_image", assets->small_image, allocator);
                assets_json.AddMember("small_text", assets->small_text, allocator);
                json.AddMember("assets", assets_json, allocator);
            }

            // Add secrets
            if (secrets) {
                rapidjson::Document secrets_json(rapidjson::kObjectType);
                secrets_json.AddMember("join", secrets->join, allocator);
                secrets_json.AddMember("spectate", secrets->spectate, allocator);
                secrets_json.AddMember("match", secrets->match, allocator);
                json.AddMember("secrets", secrets_json, allocator);
            }

            if (instance) {
                json.AddMember("instance", instance, allocator);
            }

            if (flags != 0 ) {
                json.AddMember("flags", flags, allocator);
            }
            
            return json;
        }

        rapidjson::Document ToJson() const {
            rapidjson::Document json(rapidjson::kObjectType);
            json.AddMember("name", name, json.GetAllocator());
            json.AddMember("type", (int) type, json.GetAllocator());
            if (!url.empty()) {
                json.AddMember("url", url, json.GetAllocator());
            }
            if (std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count() != 0) {
                json.AddMember("created_at", std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count(), json.GetAllocator());
            }

            auto start_it = timestamps.find("start");
            if (start_it != timestamps.end()) {
                json.AddMember("start", std::chrono::duration_cast<std::chrono::seconds>(start_it->second.time_since_epoch()).count(), json.GetAllocator());
            }

            auto end_it = timestamps.find("end");
            if (end_it != timestamps.end()) {
                json.AddMember("end", std::chrono::duration_cast<std::chrono::seconds>(end_it->second.time_since_epoch()).count(), json.GetAllocator());
            }

            if (!application_id.empty()) {
                json.AddMember("application_id", application_id, json.GetAllocator());
            }

            if (!details.empty()) {
                json.AddMember("details", details, json.GetAllocator());
            }

            if (!state.empty()) {
                json.AddMember("state", state, json.GetAllocator());
            }

            // Create an Activity Emoji
            if (!emoji.name.empty() || emoji.id != 0) {
                rapidjson::Document activity_emoji(rapidjson::kObjectType);
                activity_emoji.AddMember("name", emoji.name, json.GetAllocator());
                activity_emoji.AddMember("id", emoji.id, json.GetAllocator());
                activity_emoji.AddMember("animated", emoji.animated, json.GetAllocator());
                json.AddMember("emoji", activity_emoji, json.GetAllocator());
            }

            // Add party
            if (party) {
                rapidjson::Document party_json(rapidjson::kObjectType);
                party_json.AddMember("id", party->id, json.GetAllocator());
                party_json.AddMember("current_size", party->current_size, json.GetAllocator());
                party_json.AddMember("max_size", party->max_size, json.GetAllocator());
                json.AddMember("party", party_json, json.GetAllocator());
            }

            // Add assets
            if (assets) {
                rapidjson::Document assets_json(rapidjson::kObjectType);
                assets_json.AddMember("large_image", assets->large_image, json.GetAllocator());
                assets_json.AddMember("large_text", assets->large_text, json.GetAllocator());
                assets_json.AddMember("small_image", assets->small_image, json.GetAllocator());
                assets_json.AddMember("small_text", assets->small_text, json.GetAllocator());
                json.AddMember("assets", assets_json, json.GetAllocator());
            }

            // Add secrets
            if (secrets) {
                rapidjson::Document secrets_json(rapidjson::kObjectType);
                secrets_json.AddMember("join", secrets->join, json.GetAllocator());
                secrets_json.AddMember("spectate", secrets->spectate, json.GetAllocator());
                secrets_json.AddMember("match", secrets->match, json.GetAllocator());
                json.AddMember("secrets", secrets_json, json.GetAllocator());
            }

            if (instance) {
                json.AddMember("instance", instance, json.GetAllocator());
            }

            if (flags != 0) {
                json.AddMember("flags", flags, json.GetAllocator());
            }
            
            return json;
        }

        std::string name;
        discpp::Activity::ActivityType type;
        std::string url;
        std::chrono::system_clock::time_point created_at;
        std::unordered_map<std::string, std::chrono::system_clock::time_point> timestamps;
        std::string application_id;
        std::string details;
        std::string state;
        discpp::Emoji emoji;
        std::shared_ptr<discpp::Activity::Party> party;
        std::shared_ptr<discpp::Activity::Assets> assets;
        std::shared_ptr<discpp::Activity::Secrets> secrets;
        bool instance = false;
        int flags = 0;
    };

	class Presence { // @TODO: Add documentation.
	public:
	    Presence() = default;
		Presence(discpp::Client* client, rapidjson::Document& json) {
		    status = json["status"].GetString();
            for (auto const& activity : json["activities"].GetArray()) {
                rapidjson::Document activity_json(rapidjson::kObjectType);
                activity_json.CopyFrom(activity, activity_json.GetAllocator());

                activities.emplace_back(client, activity_json);
            }
		}

		Presence(const std::string& text, const discpp::Activity::ActivityType& type, const std::string& status = "online", const bool afk = false,
		        const std::string& url = "") : status(status), afk(afk) {
            
            discpp::Activity activity;
		    activity.name = text;
		    activity.type = type;
		    activity.url = url;
            //activity.created_at = std::chrono::system_clock::now();
            activities.push_back(activity);
		}

		Presence(std::vector<discpp::Activity> activities, const bool afk, const std::string& status) : activities(activities), afk(afk), status(status) {}

        rapidjson::Document ToJson() {
            rapidjson::Document json(rapidjson::kObjectType);
            json.AddMember("status", status, json.GetAllocator());
            json.AddMember("afk", afk, json.GetAllocator());

            rapidjson::Document activities_json(rapidjson::kArrayType);
            for (const discpp::Activity& activity : activities) {
                activities_json.PushBack(activity.ToJson(json.GetAllocator()), json.GetAllocator());
            }
            json.AddMember("activities", activities_json, json.GetAllocator());

		    return json;
		}

		std::unique_ptr<rapidjson::Document> ToJsonPtr() {
            auto json = std::make_unique<rapidjson::Document>(rapidjson::kObjectType);
            json->AddMember("status", status, json->GetAllocator());
            json->AddMember("afk", afk, json->GetAllocator());

            rapidjson::Document activities_json(rapidjson::kArrayType);
            for (const discpp::Activity& activity : activities) {
                activities_json.PushBack(activity.ToJson(json->GetAllocator()), json->GetAllocator());
            }
            json->AddMember("activities", activities_json, json->GetAllocator());

		    return json;
		}

		std::string status;
		std::vector<discpp::Activity> activities;
		bool afk;
	};
}

#endif