#include "member.h"
#include "utils.h"
#include "bot.h"
#include "role.h"

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
			roles.push_back(role.get<snowflake>());
		}
		joined_at = json["joined_at"];
		premium_since = GetDataSafely<std::string>(json, "premium_since");
		deaf = json["deaf"].get<bool>();
		mute = json["mute"].get<bool>();
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
		SendPatchRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders(), body);
	}
}