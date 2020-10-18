#ifndef DISCPP_DM_CHANNEL_UPDATE_EVENT_H
#define DISCPP_DM_CHANNEL_UPDATE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class ChannelUpdateEvent : public Event {
	public:
		inline ChannelUpdateEvent(Shard& shard, discpp::Channel channel) : Event(shard), channel(channel) {}

        discpp::Channel channel;

        virtual int GetEventType() const override {
            return 5;
        }
	};
}

#endif