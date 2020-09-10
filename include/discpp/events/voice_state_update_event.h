#ifndef DISCPP_VOICE_STATE_UPDATE_EVENT_H
#define DISCPP_VOICE_STATE_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

namespace discpp {
	class VoiceStateUpdateEvent : public Event {
	public:
		inline VoiceStateUpdateEvent(Shard& shard, rapidjson::Document& json) : Event(shard), json(json) { }

		rapidjson::Document& json;

        virtual int GetEventType() const override {
            return 34;
        }
	};
}

#endif