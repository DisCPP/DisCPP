#ifndef DISCPP_WEBHOOKS_UPDATE_EVENT_H
#define DISCPP_WEBHOOKS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class WebhooksUpdateEvent : public Event {
	public:
		inline WebhooksUpdateEvent(discpp::Channel channel) : channel(channel) {}

		discpp::Channel channel;
	};
}

#endif