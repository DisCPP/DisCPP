#ifndef DISCPP_WEBHOOKS_UPDATE_EVENT_H
#define DISCPP_WEBHOOKS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class WebhooksUpdateEvent : public Event {
	public:
		inline WebhooksUpdateEvent(discord::Channel channel) : channel(channel) {}

		discord::Channel channel;
	};
}

#endif