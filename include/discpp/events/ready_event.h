#ifndef DISCPP_READY_EVENT_H
#define DISCPP_READY_EVENT_H

#include "../event.h"

namespace discord {
	class ReadyEvent : public Event {
	public:
		ReadyEvent() = default;
	};
}

#endif