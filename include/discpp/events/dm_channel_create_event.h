#ifndef DISCPP_DM_CHANNEL_CREATE_EVENT_H
#define DISCPP_DM_CHANNEL_CREATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class DMChannelCreateEvent : public Event {
	public:
		inline DMChannelCreateEvent(discpp::DMChannel dm_channel) : dm_channel(dm_channel) {}

		discpp::DMChannel dm_channel;
	};
}

#endif