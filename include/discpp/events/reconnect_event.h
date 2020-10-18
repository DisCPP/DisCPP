#ifndef DISCPP_RECONNECT_EVENT_H
#define DISCPP_RECONNECT_EVENT_H

#include "../event.h"

namespace discpp {
	class ReconnectEvent : public Event {
	public:
		ReconnectEvent(Shard& shard) : Event(shard) {}

        virtual int GetEventType() const override {
            return 29;
        }
	};
}

#endif