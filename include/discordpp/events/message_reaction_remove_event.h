#ifndef DISCORDPP_MESSAGE_REACTION_REMOVE_EVENT_H
#define DISCORDPP_MESSAGE_REACTION_REMOVE_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../emoji.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageReactionRemoveEvent : public Event {
	public:
		inline MessageReactionRemoveEvent(discord::Message message, discord::Emoji emoji, discord::User user) : message(message), emoji(emoji), user(user) {}

		discord::Message message;
		discord::Emoji emoji;
		discord::User user;
	};
}

#endif