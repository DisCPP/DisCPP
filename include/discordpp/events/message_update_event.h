#ifndef DISCORDPP_MESSAGE_UPDATE_EVENT_H
#define DISCORDPP_MESSAGE_UPDATE_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageUpdateEvent : public Event {
	public:
		inline MessageUpdateEvent(discord::Message message, discord::Message old_message, bool triggered_from_edit) : message(message), old_message(old_message), triggered_from_edit(triggered_from_edit) {}

		discord::Message message;
		discord::Message old_message;
		bool triggered_from_edit;
	};
}

#endif