#ifndef DISCPP_READY_EVENT_H
#define DISCPP_READY_EVENT_H

#include "../event.h"

namespace discpp {
	class ReadyEvent : public Event {
	public:
		inline explicit ReadyEvent(const discpp::JsonObject& json) : payload(json) {

		}

        discpp::JsonObject payload;
	};
}

#endif