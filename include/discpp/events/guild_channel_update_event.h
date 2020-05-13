#ifndef DISCPP_GUILD_CHANNEL_UPDATE_EVENT_H
#define DISCPP_GUILD_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class GuildChannelUpdateEvent : public Event {
	public:
		inline GuildChannelUpdateEvent(std::shared_ptr<discpp::GuildChannel> channel) : channel(channel) {}

		std::shared_ptr<discpp::GuildChannel> channel;
	};
}

#endif