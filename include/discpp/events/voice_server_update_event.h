#ifndef DISCPP_VOICE_SERVER_UPDATE_EVENT_H
#define DISCPP_VOICE_SERVER_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

namespace discpp {
	class VoiceServerUpdateEvent : public Event {
	public:
		inline explicit VoiceServerUpdateEvent(const discpp::JsonObject& json) : json(json) { }

		discpp::JsonObject json;
	};
}

#endif