#ifndef DISCPP_READY_EVENT_H
#define DISCPP_READY_EVENT_H

#include "../event.h"

namespace discpp {
	class ReadyEvent : public Event {
	public:
		ReadyEvent() = default;
	};
}

#endif