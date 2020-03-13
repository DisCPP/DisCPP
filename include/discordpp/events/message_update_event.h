#ifndef DISCORDPP_MESSAGE_UPDATE_EVENT_H
#define DISCORDPP_MESSAGE_UPDATE_EVENT_H

#include "event.h"
#include "message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageUpdateEvent : public Event {
	public:
		inline MessageUpdateEvent(nlohmann::json json) : message(discord::Message(json)) { }

		discord::Message message;
	};
}

#endif