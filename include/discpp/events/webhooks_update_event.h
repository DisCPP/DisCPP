#ifndef DISCPP_WEBHOOKS_UPDATE_EVENT_H
#define DISCPP_WEBHOOKS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class WebhooksUpdateEvent : public Event {
	public:
		inline WebhooksUpdateEvent(Shard& shard, discpp::Channel channel) : Event(shard), channel(channel) {}

		discpp::Channel channel;

        virtual int GetEventType() const override {
            return 35;
        }
	};
}

#endif