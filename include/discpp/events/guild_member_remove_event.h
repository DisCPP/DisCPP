#ifndef DISCPP_GUILD_MEMBER_REMOVE_EVENT_H
#define DISCPP_GUILD_MEMBER_REMOVE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../member.h"



namespace discpp {
	class GuildMemberRemoveEvent : public Event {
	public:
		inline GuildMemberRemoveEvent(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Member> member) : guild(guild), member(member) {}

		std::shared_ptr<discpp::Guild> guild;
		std::shared_ptr<discpp::Member> member;

        virtual int GetEventType() const override {
            return 13;
        }
	};
}

#endif