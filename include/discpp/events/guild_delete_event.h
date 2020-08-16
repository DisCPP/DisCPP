#ifndef DISCPP_GUILD_DELETE_EVENT_H
#define DISCPP_GUILD_DELETE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildDeleteEvent : public Event {
	public:
		inline GuildDeleteEvent(std::shared_ptr<discpp::Guild> guild) : guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;

        virtual int GetEventType() const override {
            return 9;
        }
	};
}

#endif