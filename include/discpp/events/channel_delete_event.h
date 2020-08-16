#ifndef DISCPP_DM_CHANNEL_DELETE_EVENT_H
#define DISCPP_DM_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class ChannelDeleteEvent : public Event {
	public:
		inline ChannelDeleteEvent(Shard& shard, discpp::Channel channel) : Event(shard), channel(channel) {}

		discpp::Channel channel;

        virtual int GetEventType() const override {
            return 3;
        }
	};
}

#endif