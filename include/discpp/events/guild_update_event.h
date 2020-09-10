#ifndef DISCPP_GUILD_UPDATE_EVENT_H
#define DISCPP_GUILD_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildUpdateEvent : public Event {
	public:
		inline GuildUpdateEvent(Shard& shard, std::shared_ptr<discpp::Guild> guild) : Event(shard), guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;

        virtual int GetEventType() const override {
            return 19;
        }
	};
}

#endif