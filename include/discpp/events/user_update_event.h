#ifndef DISCPP_USER_UPDATE_EVENT_H
#define DISCPP_USER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"



namespace discpp {
	class UserUpdateEvent : public Event {
	public:
		inline UserUpdateEvent(Shard& shard, discpp::User user) : Event(shard), user(user) {}

		discpp::User user;

        virtual int GetEventType() const override {
            return 32;
        }
	};
}

#endif