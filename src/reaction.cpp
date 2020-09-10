#include "reaction.h"

namespace discpp {
	Reaction::Reaction(discpp::Client* client, rapidjson::Document& json) {
		count = json["count"].GetInt();
		from_bot = json["me"].GetBool();

		emoji = discpp::Emoji(client, *GetDocumentInsideJson(json, "emoji"));
	}
}