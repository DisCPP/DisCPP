#ifndef DISCPP_GUILD_BAN_ADD_EVENT_H
#define DISCPP_GUILD_BAN_ADD_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../user.h"



namespace discpp {
	class GuildBanAddEvent : public Event {
	public:
		inline GuildBanAddEvent(Shard& shard, discpp::Guild guild, discpp::User user) : Event(shard), guild(guild), user(user) {}

		discpp::Guild guild;
		discpp::User user;

        virtual int GetEventType() const override {
            return 6;
        }
	};
}

#endif