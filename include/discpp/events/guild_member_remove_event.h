#ifndef DISCPP_GUILD_MEMBER_REMOVE_EVENT_H
#define DISCPP_GUILD_MEMBER_REMOVE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildMemberRemoveEvent : public Event {
	public:
		inline GuildMemberRemoveEvent(discord::Guild guild, discord::Member member) : guild(guild), member(member) {}

		discord::Guild guild;
		discord::Member member;
	};
}

#endif