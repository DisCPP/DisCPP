#ifndef DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H
#define DISCPP_GUILD_EMOJIS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildEmojisUpdateEvent : public Event {
	public:
		inline GuildEmojisUpdateEvent(Shard& shard, std::shared_ptr<discpp::Guild> guild) : Event(shard), guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;

        virtual int GetEventType() const override {
            return 10;
        }
	};
}

#endif