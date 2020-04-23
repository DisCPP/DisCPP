#ifndef DISCPP_CHANNEL_PINS_UPDATE_EVENT_H
#define DISCPP_CHANNEL_PINS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class ChannelPinsUpdateEvent : public Event {
	public:
		inline ChannelPinsUpdateEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif