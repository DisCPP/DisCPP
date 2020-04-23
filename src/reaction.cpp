#include "reaction.h"

namespace discpp {
	Reaction::Reaction(nlohmann::json json) {
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

		count = json["count"].get<int>();
		from_bot = json["me"].get<bool>();
		emoji = discpp::Emoji(json["emoji"]);
	}
}