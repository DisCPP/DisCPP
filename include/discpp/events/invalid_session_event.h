#ifndef DISCPP_INVALID_SESSION_EVENT_H
#define DISCPP_INVALID_SESSION_EVENT_H

#include "../event.h"

namespace discord {
	class InvalidSessionEvent : public Event {
	public:
		InvalidSessionEvent() = default;
	};
}

#endif