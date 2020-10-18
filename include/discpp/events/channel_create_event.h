#ifndef DISCPP_DM_CHANNEL_CREATE_EVENT_H
#define DISCPP_DM_CHANNEL_CREATE_EVENT_H

#include "../event.h"
#include "../channel.h"

namespace discpp {
	class ChannelCreateEvent : public Event {
	public:
		inline ChannelCreateEvent(Shard& shard, discpp::Channel channel) : Event(shard), channel(channel) {}

        discpp::Channel channel;

        virtual int GetEventType() const override {
            return 2;
        }
	};
}

#endif