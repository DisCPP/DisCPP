#ifndef DISCPP_DM_CHANNEL_UPDATE_EVENT_H
#define DISCPP_DM_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class ChannelUpdateEvent : public Event {
	public:
		inline ChannelUpdateEvent(discpp::Channel channel) : channel(channel) {}

        discpp::Channel channel;
	};
}

#endif