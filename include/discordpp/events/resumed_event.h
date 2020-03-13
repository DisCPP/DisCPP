#ifndef DISCORDPP_RESUMED_EVENT_H
#define DISCORDPP_RESUMED_EVENT_H

#include "event.h"

namespace discord {
	class ResumedEvent : public Event {
	public:
		ResumedEvent() = default;
	};
}

#endif