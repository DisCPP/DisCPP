#include "reaction.h"
#include "emoji.h"

namespace discord {
	Reaction::Reaction(nlohmann::json json) {
		count = json["count"].get<int>();
		from_bot = json["me"].get<bool>();
		emoji = discord::Emoji(json["emoji"]);
	}
}