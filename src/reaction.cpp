#include "reaction.h"

namespace discord {
	Reaction::Reaction(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Reaction object by parsing json.
		 *
		 * ```cpp
		 *      discord::Reaction reaction(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the reaction object.
		 *
		 * @return discord::Reaction, this is a constructor.
		 */

		count = json["count"].get<int>();
		from_bot = json["me"].get<bool>();
		emoji = discord::Emoji(json["emoji"]);
	}
}