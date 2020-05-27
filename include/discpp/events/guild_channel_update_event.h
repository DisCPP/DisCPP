#ifndef DISCPP_GUILD_CHANNEL_UPDATE_EVENT_H
#define DISCPP_GUILD_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class GuildChannelUpdateEvent : public Event {
	public:
		inline GuildChannelUpdateEvent(discpp::GuildChannel channel) : channel(channel) {}

		discpp::GuildChannel channel;
	};
}

#endif