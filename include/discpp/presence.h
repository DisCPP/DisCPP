#ifndef DISCPP_ACTIVITY_H
#define DISCPP_ACTIVITY_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

#include <string>
#include <ctime>

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
                id = GetDataSafely<std::string>(json, "id");
                if (ContainsNotNull(json, "size")) {
                    current_size = json["size"].GetArray()[0].GetInt();
                    max_size = json["size"].GetArray()[1].GetInt();
                }
            }

            std::string id;
            int current_size;
            int max_size;
        };

        struct Assets {
            Assets() = default;
            Assets(rapidjson::Document& json) {
                large_image = GetDataSafely<std::string>(json, "large_image");
                large_text = GetDataSafely<std::string>(json, "large_text");
                small_image = GetDataSafely<std::string>(json, "small_image");
                small_text = GetDataSafely<std::string>(json, "small_text");
            }

            std::string large_image;
            std::string large_text;
            std::string small_image;
            std::string small_text;
        };

        struct Secrets {
            Secrets() = default;
            Secrets(rapidjson::Document& json) {
                join = GetDataSafely<std::string>(json, "join");
                spectate = GetDataSafely<std::string>(json, "spectate");
                match = GetDataSafely<std::string>(json, "match");
            }

            std::string join;
            std::string spectate;
            std::string match;
        };

        Activity() = default;
        Activity(rapidjson::Document& json) {
            name = json["name"].GetString();
            type = static_cast<ActivityType>(json["type"].GetInt());
            url = GetDataSafely<std::string>(json, "url");
            created_at = json["created_at"].Get<std::time_t>();
            if (ContainsNotNull(json, "timestamps")) {
                rapidjson::Document timestamps_json(rapidjson::kObjectType);
                timestamps_json.CopyFrom(json["timestamps"], timestamps_json.GetAllocator());

                if (ContainsNotNull(timestamps_json, "start")) {
                    this->timestamps.emplace("start", timestamps_json["start"].Get<std::time_t>());
                }

                if (ContainsNotNull(timestamps_json, "end")) {
                    this->timestamps.emplace("end", timestamps_json["end"].Get<std::time_t>());
                }
            }
            application_id = json["id"].GetString();
            details = GetDataSafely<std::string>(json, "details");
            state = GetDataSafely<std::string>(json, "state");
            emoji = ConstructDiscppObjectFromJson(json, "emoji", discpp::Emoji());
            party = ConstructDiscppObjectFromJson(json, "party", discpp::Activity::Party());
            assets = ConstructDiscppObjectFromJson(json, "assets", discpp::Activity::Assets());
            secrets = ConstructDiscppObjectFromJson(json, "secrets", discpp::Activity::Secrets());
            instance = GetDataSafely<bool>(json, "instance");
            flags = GetDataSafely<int>(json, "flags");
        }

        std::string name;
        discpp::Activity::ActivityType type;
        std::string url;
        std::time_t created_at;
        std::unordered_map<std::string, std::time_t> timestamps;
        std::string application_id;
        std::string details;
        std::string state;
        discpp::Emoji emoji;
        discpp::Activity::Party party;
        discpp::Activity::Assets assets;
        discpp::Activity::Secrets secrets;
        bool instance;
        int flags;
    };

	class Presence {
	public:
	    Presence() = default;
		Presence(rapidjson::Document& json) {
		    status = json["status"].GetString();
		    game = ConstructDiscppObjectFromJson(json, "game", discpp::Activity());
            for (auto const& activity : json["activities"].GetArray()) {
                rapidjson::Document activity_json(rapidjson::kObjectType);
                activity_json.CopyFrom(activity, activity_json.GetAllocator());

                activities.emplace_back(activity_json);
            }
		}

		Presence(const std::string& text, const discpp::Activity::ActivityType& type, const std::string& status = "online", const bool& afk = false, const std::string& url = "") : status(status), afk(afk) {
		    game.name = text;
		    game.type = type;
		    game.url = url;
		}

		Presence(const discpp::Activity& activity, const bool& afk, const std::string& status) : game(activity), afk(afk), status(status) {}

		rapidjson::Document ToJson() {
            rapidjson::Document result;

            std::string str_activity = "{\"status\": \"" + status + "\", \"afk\": " + (afk ? "true" : "false") + ", \"game\": " + \
                    "{\"name\": \"" + game.name + "\", \"type\": " + std::to_string(static_cast<int>(game.type)) + ((!game.url.empty()) ? ", \"url\": \"" + game.url + "\"" : "") + "}, \"since\": \"" + std::to_string(time(NULL) - 10) + "\"}";
            result.Parse(str_activity.c_str());

			return std::move(result);
		}

		std::string status;
		discpp::Activity game;
		std::vector<discpp::Activity> activities;
		bool afk;
	};
}

#endif