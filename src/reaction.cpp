#include "reaction.h"

namespace discpp {
	Reaction::Reaction(rapidjson::Document& json) {
		count = json["count"].GetInt();
		from_bot = json["me"].GetBool();

		emoji = discpp::Emoji(*GetDocumentInsideJson(json, "emoji"));
	}
}