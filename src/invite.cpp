#include "invite.h"

namespace discord {
	Invite::Invite(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Emoji object by parsing json.
		 *
		 * ```cpp
		 *      discord::Invite invite(json);
		 * ```
		 *
		 * @param[in] json The that makes up the invite object.
		 *
		 * @return discord::Invite, this is a constructor.
		 */

		code = json["code"];
		guild = (json.contains("guild")) ? discord::Guild(json["guild"]["id"].get<snowflake>()) : discord::Guild();
		channel = discord::Channel(json["channel"]["id"].get<snowflake>());
		target_user = (json.contains("target_user")) ? discord::User(json["target_user"]) : discord::User();
		target_user_type = GetDataSafely<int>(json, "target_user_type");
		approximate_presence_count = GetDataSafely<int>(json, "approximate_presence_count");
		approximate_member_count = GetDataSafely<int>(json, "approximate_member_count");
	}
}