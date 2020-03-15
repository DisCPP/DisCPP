#ifndef DISCORDPP_MESSAGE_UPDATE_EVENT_H
#define DISCORDPP_MESSAGE_UPDATE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageUpdateEvent : public Event {
	public:
		inline MessageUpdateEvent(discord::Message message) : message(message) {}

		discord::Message message;
	};
}

#endif