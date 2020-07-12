#include "reaction.h"
#include "json_object.h"

namespace discpp {
	Reaction::Reaction(const discpp::JsonObject& json) {
		count = json["count"].GetInt();
		from_bot = json["me"].GetBool();

		emoji = discpp::Emoji(json["emoji"]);
	}
}