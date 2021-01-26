#ifndef DISCPP_MESSAGE_CREATE_EVENT_H
#define DISCPP_MESSAGE_CREATE_EVENT_H

#include "../event.h"
#include "../message.h"



namespace discpp {
	class MessageCreateEvent : public Event {
	public:
		inline MessageCreateEvent(Shard& shard, discpp::Message& message) : Event(shard), message(message) {}

		discpp::Message message;

        virtual int GetEventType() const override {
            return 22;
        }
	};
}

#endif