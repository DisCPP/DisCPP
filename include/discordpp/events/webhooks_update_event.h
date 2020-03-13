#ifndef DISCORDPP_WEBHOOKS_UPDATE_EVENT_H
#define DISCORDPP_WEBHOOKS_UPDATE_EVENT_H

#include "event.h"
#include "channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class WebhooksUpdateEvent : public Event {
	public:
		inline WebhooksUpdateEvent(nlohmann::json json) : channel(discord::Channel(json["channel_id"].get<snowflake>())) {}
		inline WebhooksUpdateEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif