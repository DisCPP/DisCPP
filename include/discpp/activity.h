#ifndef DISCPP_ACTIVITY_H
#define DISCPP_ACTIVITY_H

#include <string>

#include <nlohmann/json.hpp>

#include "utils.h"

namespace discord {
	typedef std::string snowflake;

	namespace presence {
		struct Status {
			inline const static std::string online = "online";
			inline const static std::string dnd = "dnd";
			inline const static std::string idle = "idle";
			inline const static std::string invisible = "invisible";
			inline const static std::string offline = "offline";
		};

		enum ActivityType : int {
			NONE = -1,
			GAME = 0,
			STREAMING = 1,
			LISTENING = 2,
			CUSTOM = 4 // Not supported for bots yet. So that makes me sad :(
		};
	}

	class Activity {
	public:
		Activity() = default;

		// Discord activity object:
		std::string text;// name
		presence::ActivityType type;
		std::string url;
		std::string created_at;
		std::string application_id;
		// end of discord activity
		std::string status;
		bool afk;

		Activity(std::string text, presence::ActivityType type, std::string status = "online", bool afk = false, std::string url = "") : text(text), type(type), status(status), afk(afk), url(url) {}

		nlohmann::json ToJson() {
			nlohmann::json json = nlohmann::json({
				{"since", nullptr},
				{"game", {
					{"name", text},
					{"type", static_cast<int>(type)}}
				},
				{"status", status},
				{"afk", std::to_string(afk)}
			});

			if (!url.empty()) {
				json["game"]["url"] = url;
			}
			return json;
		}
	};
}

#endif