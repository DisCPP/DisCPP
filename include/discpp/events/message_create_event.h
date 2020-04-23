#ifndef DISCPP_MESSAGE_CREATE_EVENT_H
#define DISCPP_MESSAGE_CREATE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageCreateEvent : public Event {
	public:
		inline MessageCreateEvent(discord::Message message) : message(message) {}

		discord::Message message;
	};
}

#endif