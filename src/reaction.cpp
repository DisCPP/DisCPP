#include "reaction.h"

namespace discpp {
	Reaction::Reaction(rapidjson::Document& json) {
		count = json["count"].GetInt();
		from_bot = json["me"].GetBool();

		rapidjson::Document emoji_json = GetDocumentInsideJson(json, "emoji");
		emoji = discpp::Emoji(emoji_json);
	}
}