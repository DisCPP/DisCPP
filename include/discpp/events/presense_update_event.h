#ifndef DISCPP_PRESENSE_UDPATE_EVENT_H
#define DISCPP_PRESENSE_UDPATE_EVENT_H

#include "../event.h"
#include "../user.h"
#include "../presence.h"
namespace discpp {
	class PresenseUpdateEvent : public Event {
	public:
		inline PresenseUpdateEvent(Shard& shard, discpp::User user, discpp::Presence presence) : Event(shard), user(user), presence(presence) {}

		discpp::User user;
		discpp::Presence presence;

        virtual int GetEventType() const override {
            return 28;
        }
	};
}

#endif