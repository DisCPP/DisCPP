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
            Party(const discpp::JsonObject& json) {
                if (json.ContainsNotNull("id")) {
                    id = json["id"].GetString();
                }
                if (json.ContainsNotNull("size")) {
                    int i = 0;
                    json["size"].IterateThrough([&](const discpp::JsonObject& json)->bool {
                        if (i == 0) {
                            current_size = json.GetInt();
                        } else if (i == 1) {
                            max_size = json.GetInt();
                        }

                        i++;
                        return true;
                    });
                }
            }

            std::string id;
            int current_size;
            int max_size;
        };

        struct Assets {
            Assets() = default;
            Assets(const discpp::JsonObject& json) {
                if (json.ContainsNotNull("large_image")) {
                    large_image = json["large_image"].GetString();
                }
                if (json.ContainsNotNull("large_text")) {
                    large_text = json["large_text"].GetString();
                }
                if (json.ContainsNotNull("small_image")) {
                    small_image = json["small_image"].GetString();
                }
                if (json.ContainsNotNull("small_text")) {
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
            Secrets(const discpp::JsonObject& json) {
                if (json.ContainsNotNull("join")) {
                    join = json["join"].GetString();
                }
                if (json.ContainsNotNull("spectate")) {
                    spectate = json["spectate"].GetString();
                }
                if (json.ContainsNotNull("match")) {
                    match = json["match"].GetString();
                }
            }

            std::string join;
            std::string spectate;
            std::string match;
        };

        Activity() = default;
        Activity(discpp::JsonObject json) {
            name = json["name"].GetString();
            type = static_cast<ActivityType>(json["type"].GetInt());
            if (json.ContainsNotNull("url")) {
                url = json["url"].GetString();
            }
            created_at = std::chrono::system_clock::from_time_t(json["created_at"].Get<std::time_t>());
            if (json.ContainsNotNull("timestamps")) {
                discpp::JsonObject timestamps_json = json["timestamps"];

                this->timestamps.emplace("start", timestamps_json["start"].Get<std::time_t>());
                this->timestamps.emplace("end", timestamps_json["end"].Get<std::time_t>());
            }
            application_id = json["id"].GetString();
            if (json.ContainsNotNull("details")) {
                details = json["details"].GetString();
            }
            if (json.ContainsNotNull("state")) {
                state = json["state"].GetString();
            }
            emoji = json.ConstructDiscppObjectFromJson("emoji", discpp::Emoji());
            party = json.ConstructDiscppObjectFromJson("party", discpp::Activity::Party());
            assets = json.ConstructDiscppObjectFromJson("assets", discpp::Activity::Assets());
            secrets = json.ConstructDiscppObjectFromJson("secrets", discpp::Activity::Secrets());
            instance = json.Get<bool>("instance");
            flags = json.Get<int>("flags");
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
		Presence(const discpp::JsonObject& json) {
		    status = json["status"].GetString();
		    game = std::make_shared<discpp::Activity>(json.ConstructDiscppObjectFromJson("game", discpp::Activity()));

		    json["activities"].IterateThrough([&](const discpp::JsonObject& json)->bool {
                activities.emplace_back(json);
                return true;
		    });
		}

		Presence(const std::string& text, const discpp::Activity::ActivityType& type, const std::string& status = "online", const bool afk = false,
		        const std::string& url = "") : status(status), afk(afk) {
            game = std::make_shared<discpp::Activity>();
		    game->name = text;
		    game->type = type;
		    game->url = url;
		}

		Presence(std::shared_ptr<discpp::Activity> activity, const bool afk, const std::string& status) : game(activity), afk(afk), status(status) {}

		discpp::JsonObject ToJson() {
		    std::string str_activity = "{\"status\": \"" + status + "\", \"afk\": " + (afk ? "true" : "false") + ", \"game\": " + \
                "{\"name\": \"" + game->name + "\", \"type\": " + std::to_string(static_cast<int>(game->type)) + \
                ((!game->url.empty()) ? ", \"url\": \"" + game->url + "\"" : "") + "}, \"since\": \"" + \
                std::to_string(time(NULL) - 10) + "\"}";

	        return discpp::JsonObject(str_activity);
		}

		std::string status;
		std::shared_ptr<discpp::Activity> game;
		std::vector<discpp::Activity> activities;
		bool afk;
	};
}

#endif