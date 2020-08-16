#ifndef DISCPP_GUILD_CREATE_EVENT_H
#define DISCPP_GUILD_CREATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildCreateEvent : public Event {
	public:
		inline GuildCreateEvent(std::shared_ptr<discpp::Guild> guild) : guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;

        virtual int GetEventType() const override {
            return 8;
        }
	};
}

#endif