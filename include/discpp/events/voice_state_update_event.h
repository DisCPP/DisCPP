#ifndef DISCPP_VOICE_STATE_UPDATE_EVENT_H
#define DISCPP_VOICE_STATE_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

namespace discpp {
	class VoiceStateUpdateEvent : public Event {
	public:
		inline VoiceStateUpdateEvent(const discpp::JsonObject& json) : json(json) { }

		discpp::JsonObject json;
	};
}

#endif