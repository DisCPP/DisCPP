#include "member.h"
#include "utils.h"
#include "bot.h"

namespace discord {
	Member::Member(snowflake id) : discord::DiscordObject(id) {
		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			*this = *member;
		}
	}

	Member::Member(nlohmann::json json) {
		user = discord::User(json["user"]);
		nick = GetDataSafely<std::string>(json, "nick");
		for (auto& role : json["roles"]) {
			roles.push_back(discord::Role(role));
		}
		joined_at = json["joined_at"];
		premium_since = GetDataSafely<std::string>(json, "premium_since");
		deaf = json["deaf"].get<bool>();
		mute = json["mute"].get<bool>();
	}
}