#ifndef DISCPP_CHANNEL_PINS_UPDATE_EVENT_H
#define DISCPP_CHANNEL_PINS_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class ChannelPinsUpdateEvent : public Event {
	public:
		inline ChannelPinsUpdateEvent(Shard& shard, discpp::Channel channel) : Event(shard), channel(channel) {}

		discpp::Channel channel;

        virtual int GetEventType() const override {
            return 4;
        }
	};
}

#endif