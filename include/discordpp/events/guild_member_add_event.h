#ifndef DISCORDPP_GUILD_MEMBER_ADD_EVENT_H
#define DISCORDPP_GUILD_MEMBER_ADD_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildMemberAddEvent : public Event {
	public:
		inline GuildMemberAddEvent(nlohmann::json json) : guild(discord::Guild(json["guild_id"].get<snowflake>())), member(discord::Member(json)) {}
		inline GuildMemberAddEvent(discord::Guild guild, discord::Member member) : guild(guild), member(member) {}

		discord::Guild guild;
		discord::Member member;
	};
}

#endif