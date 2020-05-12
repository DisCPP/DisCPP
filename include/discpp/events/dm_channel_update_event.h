#ifndef DISCPP_DM_CHANNEL_UPDATE_EVENT_H
#define DISCPP_DM_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class DMChannelUpdateEvent : public Event {
	public:
		inline DMChannelUpdateEvent(discpp::DMChannel channel) : channel(channel) {}

		discpp::DMChannel channel;
	};
}

#endif