#ifndef DISCPP_MESSAGE_BULK_DELETE_EVENT_H
#define DISCPP_MESSAGE_BULK_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../client.h"

#include <vector>



namespace discpp {
	class MessageBulkDeleteEvent : public Event {
	public:
		inline MessageBulkDeleteEvent(Shard& shard, std::vector<discpp::Message> messages) : Event(shard), messages(messages) {}

		std::vector<discpp::Message> messages;

        virtual int GetEventType() const override {
            return 21;
        }
	};
}

#endif