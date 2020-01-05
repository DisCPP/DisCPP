#include "guild.h"
#include "role.h"
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

	Member::Member(nlohmann::json json, snowflake guild_id) : guild_id(guild_id){
		user = (json.contains("user")) ? discord::User(json["user"]) : discord::User();
		nick = GetDataSafely<std::string>(json, "nick");
		discord::Guild guild(guild_id);
		if (json.contains("roles")) {
			for (auto& role : json["roles"]) {
				roles.push_back(discord::Role(role, guild));
			}
		}
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		premium_since = GetDataSafely<std::string>(json, "premium_since");
		deaf = GetDataSafely<bool>(json, "deaf");
		mute = GetDataSafely<bool>(json, "mute");
	}

	void Member::ModifyMember(std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf, snowflake channel_id) {
		std::string json_roles = "[";
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			}
			else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "\"channel_id\": \"" + channel_id + "\"" + "}");
		SendPatchRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}
}