#include "invite.h"

namespace discpp {
	Invite::Invite(nlohmann::json json) {
		/**
		 * @brief Constructs a discpp::Emoji object by parsing json.
		 *
		 * ```cpp
		 *      discpp::Invite invite(json);
		 * ```
		 *
		 * @param[in] json The that makes up the invite object.
		 *
		 * @return discpp::Invite, this is a constructor.
		 */

		code = json["code"];
		guild = (json.contains("guild")) ? discpp::Guild(json["guild"]["id"].get<snowflake>()) : discpp::Guild();
		channel = discpp::Channel(json["channel"]["id"].get<snowflake>());
		target_user = (json.contains("target_user")) ? discpp::User(json["target_user"]) : discpp::User();
		target_user_type = GetDataSafely<int>(json, "target_user_type");
		approximate_presence_count = GetDataSafely<int>(json, "approximate_presence_count");
		approximate_member_count = GetDataSafely<int>(json, "approximate_member_count");
	}
}