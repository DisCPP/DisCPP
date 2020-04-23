#ifndef DISCPP_MESSAGE_CREATE_EVENT_H
#define DISCPP_MESSAGE_CREATE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class MessageCreateEvent : public Event {
	public:
		inline MessageCreateEvent(discpp::Message message) : message(message) {}

		discpp::Message message;
	};
}

#endif