#ifndef DISCPP_PRESENCE_UDPATE_EVENT_H
#define DISCPP_PRESENCE_UDPATE_EVENT_H

#include "../event.h"
#include "../user.h"
#include "../presence.h"

namespace discpp {
	class PresenceUpdateEvent : public Event {
	public:
		inline PresenceUpdateEvent(Shard& shard, discpp::User user, discpp::Presence presence) : Event(shard), user(user), presence(presence) {}

		discpp::User user;
		discpp::Presence presence;

        virtual int GetEventType() const override {
            return 28;
        }
	};
}

#endif