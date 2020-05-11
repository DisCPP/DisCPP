#include "reaction.h"

namespace discpp {
	Reaction::Reaction(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::Reaction object by parsing json.
		 *
		 * ```cpp
		 *      discpp::Reaction reaction(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the reaction object.
		 *
		 * @return discpp::Reaction, this is a constructor.
		 */

		count = json["count"].GetInt();
		from_bot = json["me"].GetBool();

		rapidjson::Document emoji_json = GetDocumentInsideJson(json, "emoji");
		emoji = discpp::Emoji(emoji_json);
	}
}