#ifndef DISCORDPP_MESSAGE_CREATED_EVENT_H
#define DISCORDPP_MESSAGE_CREATED_EVENT_H

#include "event.h"
#include "message.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Message;

	/*#define EVET_TYPE_FUNC(T) static std::string eventType(){return #T;}
	#define KDFA_DEFINE_VOID_EVENT(__NAME__)    struct __NAME__{EVET_TYPE_FUNC(__NAME__)};*/
	class MessageCreatedEvent : public Event {
		//EVET_TYPE_FUNC(MessageCreatedEvent);
	private:

	public:
		inline MessageCreatedEvent(nlohmann::json json) : message(discord::Message(json)) { }

		discord::Message message;
	};
}

#endif