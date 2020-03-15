#ifndef DISCORDPP_MESSAGE_DELETE_EVENT_H
#define DISCORDPP_MESSAGE_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageDeleteEvent : public Event {
	public:
		inline MessageDeleteEvent(discord::Message message) : message(message) {}

		discord::Message message;
	};
}

#endif