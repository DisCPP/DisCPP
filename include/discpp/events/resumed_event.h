#ifndef DISCPP_RESUMED_EVENT_H
#define DISCPP_RESUMED_EVENT_H

#include "../event.h"

namespace discord {
	class ResumedEvent : public Event {
	public:
		ResumedEvent() = default;
	};
}

#endif