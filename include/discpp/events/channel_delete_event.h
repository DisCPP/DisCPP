#ifndef DISCPP_CHANNEL_DELETE_EVENT_H
#define DISCPP_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class ChannelDeleteEvent : public Event {
	public:
		inline ChannelDeleteEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif