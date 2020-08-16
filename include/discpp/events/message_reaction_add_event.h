#ifndef DISCPP_MESSAGE_REACTION_ADD_EVENT_H
#define DISCPP_MESSAGE_REACTION_ADD_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../emoji.h"
#include "../user.h"



namespace discpp {
	class MessageReactionAddEvent : public Event {
	public:
		inline MessageReactionAddEvent(Shard& shard, discpp::Message message, discpp::Emoji emoji, discpp::User user) : Event(shard), message(message), emoji(emoji), user(user) {}

		discpp::Message message;
		discpp::Emoji emoji;
		discpp::User user;

        virtual int GetEventType() const override {
            return 24;
        }
	};
}

#endif