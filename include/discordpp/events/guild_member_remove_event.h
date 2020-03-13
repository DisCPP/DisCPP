#ifndef DISCORDPP_GUILD_MEMBER_REMOVE_EVENT_H
#define DISCORDPP_GUILD_MEMBER_REMOVE_EVENT_H

#include "event.h"
#include "guild.h"
#include "member.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildMemberRemoveEvent : public Event {
	public:
		inline GuildMemberRemoveEvent(nlohmann::json json) : guild(discord::Guild(json["guild_id"].get<snowflake>())), member(discord::Member(json["user"]["id"].get<snowflake>())) {}
		inline GuildMemberRemoveEvent(discord::Guild guild, discord::Member member) : guild(guild), member(member) {}

		discord::Guild guild;
		discord::Member member;
	};
}

#endif