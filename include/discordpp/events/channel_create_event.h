#ifndef DISCORDPP_CHANNEL_CREATE_EVENT_H
#define DISCORDPP_CHANNEL_CREATE_EVENT_H

#include "event.h"
#include "channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class ChannelCreateEvent : public Event {
	public:
		inline ChannelCreateEvent(nlohmann::json json) : channel(discord::Channel(json)) {}
		inline ChannelCreateEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif