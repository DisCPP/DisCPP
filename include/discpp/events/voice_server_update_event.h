#ifndef DISCPP_VOICE_SERVER_UPDATE_EVENT_H
#define DISCPP_VOICE_SERVER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

namespace discpp {
	class VoiceServerUpdateEvent : public Event {
	public:
		inline VoiceServerUpdateEvent(rapidjson::Document& json) : json(json) { }

		rapidjson::Document& json;

        virtual int GetEventType() const override {
            return 33;
        }
	};
}

#endif