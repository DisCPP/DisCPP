#ifndef DISCPP_MESSAGE_DELETE_EVENT_H
#define DISCPP_MESSAGE_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class MessageDeleteEvent : public Event {
	public:
		inline MessageDeleteEvent(discpp::Message message) : message(message) {}

		discpp::Message message;
	};
}

#endif