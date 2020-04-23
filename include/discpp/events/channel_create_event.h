#ifndef DISCPP_CHANNEL_CREATE_EVENT_H
#define DISCPP_CHANNEL_CREATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class ChannelCreateEvent : public Event {
	public:
		inline ChannelCreateEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif