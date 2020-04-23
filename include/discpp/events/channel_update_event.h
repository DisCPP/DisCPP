#ifndef DISCPP_CHANNEL_UPDATE_EVENT_H
#define DISCPP_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class ChannelUpdateEvent : public Event {
	public:
		inline ChannelUpdateEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif