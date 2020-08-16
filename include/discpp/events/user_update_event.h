#ifndef DISCPP_USER_UPDATE_EVENT_H
#define DISCPP_USER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"



namespace discpp {
	class UserUpdateEvent : public Event {
	public:
		inline UserUpdateEvent(discpp::User user) : user(user) {}

		discpp::User user;

        virtual int GetEventType() const override {
            return 32;
        }
	};
}

#endif