#ifndef DISCPP_RECONNECT_EVENT_H
#define DISCPP_RECONNECT_EVENT_H

#include "../event.h"

namespace discord {
	class ReconnectEvent : public Event {
	public:
		ReconnectEvent() = default;
	};
}

#endif