#ifndef DISCPP_GUILD_MEMBER_REMOVE_EVENT_H
#define DISCPP_GUILD_MEMBER_REMOVE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildMemberRemoveEvent : public Event {
	public:
		inline GuildMemberRemoveEvent(discpp::Guild guild, discpp::Member member) : guild(guild), member(member) {}

		discpp::Guild guild;
		discpp::Member member;
	};
}

#endif