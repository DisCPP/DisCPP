#ifndef DISCPP_GUILD_MEMBER_ADD_EVENT_H
#define DISCPP_GUILD_MEMBER_ADD_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildMemberAddEvent : public Event {
	public:
		inline GuildMemberAddEvent(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Member> member) : guild(guild), member(member) {}

        std::shared_ptr<discpp::Guild> guild;
        std::shared_ptr<discpp::Member> member;
	};
}

#endif