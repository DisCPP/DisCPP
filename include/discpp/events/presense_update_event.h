#ifndef DISCPP_PRESENSE_UDPATE_EVENT_H
#define DISCPP_PRESENSE_UDPATE_EVENT_H

#include "../event.h"
#include "../user.h"



namespace discpp {
	class PresenseUpdateEvent : public Event {
	public:
		inline PresenseUpdateEvent(Shard& shard, discpp::User user) : Event(shard), user(user) {}

		discpp::User user;

        virtual int GetEventType() const override {
            return 28;
        }
	};
}

#endif