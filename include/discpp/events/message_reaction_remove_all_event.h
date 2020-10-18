#ifndef DISCPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H
#define DISCPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H

#include "../event.h"
#include "../message.h"



namespace discpp {
	class MessageReactionRemoveAllEvent : public Event {
	public:
		inline MessageReactionRemoveAllEvent(Shard& shard, discpp::Message message) : Event(shard), message(message) {}

		discpp::Message message;

        virtual int GetEventType() const override {
            return 25;
        }
	};
}

#endif