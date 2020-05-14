#ifndef DISCPP_GUILD_CHANNEL_CREATE_EVENT_H
#define DISCPP_GUILD_CHANNEL_CREATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class GuildChannelCreateEvent : public Event {
	public:
		inline GuildChannelCreateEvent(discpp::GuildChannel guild_channel) : guild_channel(guild_channel) {}

		discpp::GuildChannel guild_channel;
	};
}

#endif