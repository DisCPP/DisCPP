#ifndef DISCPP_DM_CHANNEL_CREATE_EVENT_H
#define DISCPP_DM_CHANNEL_CREATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class ChannelCreateEvent : public Event {
	public:
		inline ChannelCreateEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif