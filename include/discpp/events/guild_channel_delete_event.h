#ifndef DISCPP_GUILD_CHANNEL_DELETE_EVENT_H
#define DISCPP_GUILD_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class GuildChannelDeleteEvent : public Event {
	public:
		inline GuildChannelDeleteEvent(discpp::GuildChannel channel) : channel(channel) {}

        discpp::GuildChannel channel;
	};
}

#endif