#ifndef DISCORDPP_CHANNEL_DELETE_EVENT_H
#define DISCORDPP_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class ChannelDeleteEvent : public Event {
	public:
		inline ChannelDeleteEvent(nlohmann::json json) : channel(discord::Channel(json)) {}
		inline ChannelDeleteEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif