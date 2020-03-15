#ifndef DISCORDPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H
#define DISCORDPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H

#include "../event.h"
#include "../message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageReactionRemoveAllEvent : public Event {
	public:
		inline MessageReactionRemoveAllEvent(discord::Message message) {}

		discord::Message message;
	};
}

#endif