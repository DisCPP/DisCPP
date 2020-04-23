#ifndef DISCPP_GUILD_MEMBER_UPDATE_EVENT_H
#define DISCPP_GUILD_MEMBER_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildMemberUpdateEvent : public Event {
	public:
		inline GuildMemberUpdateEvent(discord::Guild guild, discord::Member member) : guild(guild), member(member) {}

		discord::Guild guild;
		discord::Member member;
	};
}

#endif