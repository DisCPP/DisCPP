#ifndef DISCPP_GUILD_UPDATE_EVENT_H
#define DISCPP_GUILD_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildUpdateEvent : public Event {
	public:
		inline GuildUpdateEvent(std::shared_ptr<discpp::Guild> guild) : guild(guild) {}

        std::shared_ptr<discpp::Guild> guild;
	};
}

#endif