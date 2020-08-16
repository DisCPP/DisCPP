#ifndef DISCPP_READY_EVENT_H
#define DISCPP_READY_EVENT_H

#include "../event.h"

namespace discpp {
	class ReadyEvent : public Event {
	public:
        ReadyEvent(const discpp::ReadyEvent& ready_event) {
            /*payload.SetObject();
            payload.CopyFrom(ready_event.payload, payload.GetAllocator());*/
        }

		inline ReadyEvent(rapidjson::Document& json) {
		    payload.SetObject();
            payload.CopyFrom(json, payload.GetAllocator());
		}

        rapidjson::Document payload;

        virtual int GetEventType() const override {
            return 1;
        }
	};
}

#endif