#ifndef DISCORDPP_CHANNEL_H
#define DISCORDPP_CHANNEL_H

#include "discord_object.h"

#include <nlohmann/json.hpp>

#include <pplx/pplxtasks.h>

namespace discord {
	class Message;

	enum ModifyChannelValue : int {
		NAME,
		POSITION,
		TOPIC,
		NSFW,
		RATE_LIMIT,
		BITRATE,
		USER_LIMIT,
		PERMISSION_OVERWRITES,
		PARENT_ID
	};

	struct ModifyRequest {
		ModifyChannelValue key;
		std::string value;

		ModifyRequest(ModifyChannelValue key, std::string value) : key(key), value(value) {

		};
	};

	enum GetChannelsMessagesFields {
		AROUND,
		BEFORE,
		AFTER,
		LIMIT
	};

	class Channel : DiscordObject {
	public:
		Channel() = default;
		Channel(snowflake id);
		Channel(nlohmann::json json);
		Channel(nlohmann::json json, snowflake guild_id);

		discord::Message Send(std::string text, bool tts = false);
		discord::Channel Modify(ModifyRequest modify_request);
		discord::Channel Delete();
		//std::vector<discord::Message> GetChannelMessages(GetChannelsMessagesFields messages_fields = GetChannelsMessagesFields::LIMIT);
		discord::Message FindMessage(snowflake message_id);

		snowflake id;
		int type;
		snowflake guild_id;
		int position;
		// permission_overwrites;
		std::string name;
		std::string topic;
		bool nsfw;
		snowflake last_message_id;
		int bitrate;
		int user_limit;
		int rate_limit_per_user;
		// std::vector<discord::User> recipients;
		std::string icon;
		snowflake owner_id;
		snowflake application_id;
		snowflake category_id;
		std::string last_pin_timestamp; // TODO: Convert to iso8601Time
	};
}

#endif