#ifndef DISCPP_MESSAGE_REACTION_ADD_EVENT_H
#define DISCPP_MESSAGE_REACTION_ADD_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../emoji.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class MessageReactionAddEvent : public Event {
	public:
		inline MessageReactionAddEvent(discord::Message message, discord::Emoji emoji, discord::User user) : message(message), emoji(emoji), user(user) {}

		discord::Message message;
		discord::Emoji emoji;
		discord::User user;
	};
}

#endif