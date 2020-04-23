#ifndef DISCPP_TYPING_START_EVENT_H
#define DISCPP_TYPING_START_EVENT_H

#include "../event.h"
#include "../user.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class TypingStartEvent : public Event {
	public:
		inline TypingStartEvent(discpp::User user, discpp::Channel channel, int timestamp) : user(user), channel(channel), timestamp(timestamp) {}

		discpp::User user;
		discpp::Channel channel;
		int timestamp;
	};
}

#endif