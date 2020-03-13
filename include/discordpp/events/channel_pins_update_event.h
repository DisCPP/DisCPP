#ifndef DISCORDPP_CHANNEL_PINS_UPDATE_EVENT_H
#define DISCORDPP_CHANNEL_PINS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class ChannelPinsUpdateEvent : public Event {
	public:
		inline ChannelPinsUpdateEvent(nlohmann::json json) : channel(discord::Channel(json)) {}
		inline ChannelPinsUpdateEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif