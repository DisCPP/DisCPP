#ifndef DISCPP_MESSAGE_DELETE_EVENT_H
#define DISCPP_MESSAGE_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"



namespace discpp {
	class MessageDeleteEvent : public Event {
	public:
		inline MessageDeleteEvent(Shard& shard, discpp::Message message) : Event(shard), message(message) {}

		discpp::Message message;

        virtual int GetEventType() const override {
            return 23;
        }
	};
}

#endif