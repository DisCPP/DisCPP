#ifndef DISCPP_PRESENSE_UDPATE_EVENT_H
#define DISCPP_PRESENSE_UDPATE_EVENT_H

#include "../event.h"
#include "../user.h"



namespace discpp {
	class PresenseUpdateEvent : public Event {
	public:
		inline PresenseUpdateEvent(discpp::User user) : user(user) {}

		discpp::User user;

        virtual int GetEventType() const override {
            return 28;
        }
	};
}

#endif