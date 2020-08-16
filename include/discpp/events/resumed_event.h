#ifndef DISCPP_RESUMED_EVENT_H
#define DISCPP_RESUMED_EVENT_H

#include "../event.h"

namespace discpp {
	class ResumedEvent : public Event {
	public:
		ResumedEvent(Shard& shard) : Event(shard) {}

        virtual int GetEventType() const override {
            return 30;
        }
	};
}

#endif