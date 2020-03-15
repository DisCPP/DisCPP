#ifndef DISCORDPP_CHANNEL_UPDATE_EVENT_H
#define DISCORDPP_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class ChannelUpdateEvent : public Event {
	public:
		inline ChannelUpdateEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif