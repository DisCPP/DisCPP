#include "invite.h"
#include "utils.h"
#include "guild.h"
#include "channel.h"
#include "user.h"

namespace discord {
	Invite::Invite(nlohmann::json json) {
		code = json["code"];
		guild = (json.contains("guild")) ? discord::Guild(ToSnowflake(json["guild"]["id"])) : discord::Guild();
		channel = discord::Channel(ToSnowflake(json["channel"]["id"]));
		target_user = (json.contains("target_user")) ? discord::User(json["target_user"]) : discord::User();
		target_user_type = GetDataSafely<int>(json, "target_user_type");
		approximate_presence_count = GetDataSafely<int>(json, "approximate_presence_count");
		approximate_member_count = GetDataSafely<int>(json, "approximate_member_count");
	}
}