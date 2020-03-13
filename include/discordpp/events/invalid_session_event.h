#ifndef DISCORDPP_INVALID_SESSION_EVENT_H
#define DISCORDPP_INVALID_SESSION_EVENT_H

#include "event.h"

namespace discord {
	class InvalidSessionsEvent : public Event {
	public:
		InvalidSessionsEvent() = default;
	};
}

#endif