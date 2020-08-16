#ifndef DISCPP_MESSAGE_REACTION_REMOVE_EVENT_H
#define DISCPP_MESSAGE_REACTION_REMOVE_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../emoji.h"
#include "../user.h"



namespace discpp {
	class MessageReactionRemoveEvent : public Event {
	public:
		inline MessageReactionRemoveEvent(discpp::Message message, discpp::Emoji emoji, discpp::User user) : message(message), emoji(emoji), user(user) {}

		discpp::Message message;
		discpp::Emoji emoji;
		discpp::User user;

        virtual int GetEventType() const override {
            return 26;
        }
	};
}

#endif