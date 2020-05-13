#ifndef DISCPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H
#define DISCPP_GUILD_INTEGRATIONS_UPDATE_EVENT_H

#include "../event.h"
#include "../guild.h"



namespace discpp {
	class GuildIntegrationsUpdateEvent : public Event {
	public:
		inline GuildIntegrationsUpdateEvent(discpp::Guild guild) : guild(guild) {}

		discpp::Guild guild;
	};
}

#endif