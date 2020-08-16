#ifndef DISCPP_READY_EVENT_H
#define DISCPP_READY_EVENT_H

#include "../event.h"

namespace discpp {
	class ReadyEvent : public Event {
	public:
        /*ReadyEvent(const discpp::ReadyEvent& ready_event) : Event(ready_event) {
            payload.SetObject();
            payload.CopyFrom(ready_event.payload, payload.GetAllocator());
        }*/

		inline ReadyEvent(Shard& shard, rapidjson::Document& json) : Event(shard) {
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