#ifndef DISCPP_ACTIVITY_H
#define DISCPP_ACTIVITY_H

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
                if (ContainsNotNull(json, "id")) {
                    id = json["id"].GetString();
                }
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

            std::string join;
            std::string spectate;
            std::string match;
        };

        Activity() = default;
        Activity(rapidjson::Document& json) {
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
                    this->timestamps.emplace("start", timestamps_json["start"].Get<std::time_t>());
                }

                if (ContainsNotNull(timestamps_json, "end")) {
                    this->timestamps.emplace("end", timestamps_json["end"].Get<std::time_t>());
                }
            }
            application_id = json["id"].GetString();
            if (ContainsNotNull(json, "details")) {
                details = json["details"].GetString();
            }
            if (ContainsNotNull(json, "state")) {
                state = json["state"].GetString();
            }
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
        std::chrono::system_clock::time_point created_at;
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
		    game = std::make_shared<discpp::Activity>(ConstructDiscppObjectFromJson(json, "game", discpp::Activity()));
            for (auto const& activity : json["activities"].GetArray()) {
                rapidjson::Document activity_json(rapidjson::kObjectType);
                activity_json.CopyFrom(activity, activity_json.GetAllocator());

                activities.emplace_back(activity_json);
            }
		}

		Presence(const std::string& text, const discpp::Activity::ActivityType& type, const std::string& status = "online", const bool& afk = false,
		        const std::string& url = "") : status(status), afk(afk) {
            game = std::make_shared<discpp::Activity>();
		    game->name = text;
		    game->type = type;
		    game->url = url;
		}

		Presence(std::shared_ptr<discpp::Activity> activity, const bool& afk, const std::string& status) : game(activity), afk(afk), status(status) {}

		rapidjson::Document ToJson() {
		    std::string str_activity = "{\"status\": \"" + status + "\", \"afk\": " + (afk ? "true" : "false") + ", \"game\": " + \
                "{\"name\": \"" + game->name + "\", \"type\": " + std::to_string(static_cast<int>(game->type)) + \
                ((!game->url.empty()) ? ", \"url\": \"" + game->url + "\"" : "") + "}, \"since\": \"" + \
                std::to_string(time(NULL) - 10) + "\"}";

            rapidjson::Document result(rapidjson::kObjectType);
		    result.Parse(str_activity.c_str());

#ifndef __INTELLISENSE__
            return std::move(result);
#endif
		}

		std::string status;
		std::shared_ptr<discpp::Activity> game;
		std::vector<discpp::Activity> activities;
		bool afk;
	};
}

#endif