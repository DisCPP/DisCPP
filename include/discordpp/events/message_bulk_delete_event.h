#ifndef DISCORDPP_MESSAGE_BULK_DELETE_EVENT_H
#define DISCORDPP_MESSAGE_BULK_DELETE_EVENT_H

#include "../event.h"
#include "../message.h"
#include "../bot.h"

#include <vector>

#include <nlohmann/json.hpp>

namespace discord {
	class MessageBulkDeleteEvent : public Event {
	public:
		inline MessageBulkDeleteEvent(Bot* bot, nlohmann::json json) { 
			for (auto id : json["ids"]) {
				discord::Message message(id.get<snowflake>());
				message.channel = discord::Channel(json["channel_id"].get<snowflake>());
				if (json.contains("guild_id")) message.guild = discord::Guild(json["guild_id"].get<snowflake>());

				messages.push_back(message);
			}
		}

		inline MessageBulkDeleteEvent(std::vector<discord::Message> message) : messages(messages) {}

		std::vector<discord::Message> messages;
	};
}

#endif