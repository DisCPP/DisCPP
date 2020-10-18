#ifndef DISCPP_VOICE_STATE_UPDATE_EVENT_H
#define DISCPP_VOICE_STATE_UPDATE_EVENT_H

#include "../event.h"
#include "../user.h"

namespace discpp {
	class VoiceStateUpdateEvent : public Event {
	public:
		inline VoiceStateUpdateEvent(Shard& shard, const VoiceState voice_state) : Event(shard), voice_state(voice_state) { }

		const VoiceState voice_state;

        virtual int GetEventType() const override {
            return 34;
        }
	};
}

#endif