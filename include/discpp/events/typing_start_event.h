#ifndef DISCPP_TYPING_START_EVENT_H
#define DISCPP_TYPING_START_EVENT_H

#include "../event.h"
#include "../user.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discord {
	class TypingStartEvent : public Event {
	public:
		inline TypingStartEvent(discord::User user, discord::Channel channel, int timestamp) : user(user), channel(channel), timestamp(timestamp) {}

		discord::User user;
		discord::Channel channel;
		int timestamp;
	};
}

#endif