#include "user.h"
#include "bot.h"
#include "member.h"

namespace discord {
	User::User(snowflake id) : discord::DiscordObject(id) {
		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			*this = member->user;
		}
	}

	User::User(nlohmann::json json) {
		id = json["id"].get<snowflake>();
		username = json["username"];
		discriminator = json["discriminator"];
		avatar = GetDataSafely<std::string>(json, "avatar");
		bot = GetDataSafely<bool>(json, "bot");
		system = GetDataSafely<bool>(json, "system");
		mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
		locale = GetDataSafely<std::string>(json, "locale");
		verified = GetDataSafely<bool>(json, "verified");
		flags = GetDataSafely<int>(json, "flags");
		premium_type = static_cast<discord::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
	}
}