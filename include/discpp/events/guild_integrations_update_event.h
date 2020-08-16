#ifndef DISCPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H
#define DISCPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildIntegrationsUpdateEvent : public Event {
	public:
		inline GuildIntegrationsUpdateEvent(Shard& shard, discpp::Guild guild) : Event(shard), guild(guild) {}

		discpp::Guild guild;

        virtual int GetEventType() const override {
            return 11;
        }
	};
}

#endif