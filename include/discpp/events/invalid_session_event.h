#ifndef DISCPP_INVALID_SESSION_EVENT_H
#define DISCPP_INVALID_SESSION_EVENT_H

#include "../event.h"

namespace discpp {
	class InvalidSessionEvent : public Event {
	public:
		InvalidSessionEvent(Shard& shard) : Event(shard) {}

        virtual int GetEventType() const override {
            return 20;
        }
	};
}

#endif