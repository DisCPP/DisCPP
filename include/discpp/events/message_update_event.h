#ifndef DISCPP_MESSAGE_UPDATE_EVENT_H
#define DISCPP_MESSAGE_UPDATE_EVENT_H

#include "../event.h"
#include "../message.h"



namespace discpp {
	class MessageUpdateEvent : public Event {
	public:
		inline MessageUpdateEvent(Shard& shard, discpp::Message message, discpp::Message old_message, bool triggered_from_edit) : Event(shard), message(message), old_message(old_message), triggered_from_edit(triggered_from_edit) {}

		discpp::Message message;
		discpp::Message old_message;
		bool triggered_from_edit;

        virtual int GetEventType() const override {
            return 27;
        }
	};
}

#endif