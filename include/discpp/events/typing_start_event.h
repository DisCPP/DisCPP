#ifndef DISCPP_TYPING_START_EVENT_H
#define DISCPP_TYPING_START_EVENT_H

#include "../event.h"
#include "../user.h"
#include "../channel.h"



namespace discpp {
	class TypingStartEvent : public Event {
	public:
		inline TypingStartEvent(Shard& shard, discpp::User user, discpp::Channel channel, int timestamp) : Event(shard), user(user), channel(channel), timestamp(timestamp) {}

		discpp::User user;
        discpp::Channel channel;
		int timestamp;

        virtual int GetEventType() const override {
            return 31;
        }
	};
}

#endif