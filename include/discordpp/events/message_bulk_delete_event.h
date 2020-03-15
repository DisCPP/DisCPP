#ifndef DISCORDPP_MESSAGE_BULK_DELETE_EVENT_H
#define DISCORDPP_MESSAGE_BULK_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../bot.h"

#include <vector>

#include <nlohmann/json.hpp>

namespace discord {
	class MessageBulkDeleteEvent : public Event {
	public:
		inline MessageBulkDeleteEvent(std::vector<discord::Message> message) : messages(messages) {}

		std::vector<discord::Message> messages;
	};
}

#endif