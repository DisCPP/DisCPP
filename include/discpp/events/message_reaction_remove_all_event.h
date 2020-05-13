#ifndef DISCPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H
#define DISCPP_MESSAGE_REACTION_REMOVE_ALL_EVENT_H

#include "../event.h"
#include "../message.h"



namespace discpp {
	class MessageReactionRemoveAllEvent : public Event {
	public:
		inline MessageReactionRemoveAllEvent(discpp::Message message) {}

		discpp::Message message;
	};
}

#endif