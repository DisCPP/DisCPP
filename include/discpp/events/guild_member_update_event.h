#ifndef DISCPP_GUILD_MEMBER_UPDATE_EVENT_H
#define DISCPP_GUILD_MEMBER_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"



namespace discpp {
	class GuildMemberUpdateEvent : public Event {
	public:
		inline GuildMemberUpdateEvent(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Member> member) : guild(guild), member(member) {}

		std::shared_ptr<discpp::Guild> guild;
		std::shared_ptr<discpp::Member> member;
	};
}

#endif