#ifndef DISCPP_GUILD_BAN_REMOVE_EVENT_H
#define DISCPP_GUILD_BAN_REMOVE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../user.h"



namespace discpp {
	class GuildBanRemoveEvent : public Event {
	public:
		inline GuildBanRemoveEvent(discpp::Guild guild, discpp::User user) : guild(guild), user(user) {}

		discpp::Guild guild;
		discpp::User user;

        virtual int GetEventType() const override {
            return 7;
        }
	};
}

#endif