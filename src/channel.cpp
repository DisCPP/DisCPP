#include "channel.h"
#include "utils.h"
#include "bot.h"

#include <cpprest/http_client.h>

namespace discord {
	Channel::Channel(snowflake id) : discord::DiscordObject(id) {
		auto channel = std::find_if(discord::globals::bot_instance->channels.begin(), discord::globals::bot_instance->channels.end(), [id](discord::Channel a) { return id == a.id; });
		
		if (channel != discord::globals::bot_instance->channels.end()) {
			*this = *channel;
		}
	}

	Channel::Channel(nlohmann::json json) : discord::DiscordObject(discord::ToSnowflake(json["id"])) {
		id = ToSnowflake(json["id"]);
		type = json["type"].get<int>();
		guild_id = GetSnowflakeSafely(json, "guild_id");
		position = GetDataSafely<int>(json, "position");
		// permission_overwrites?
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		nsfw = GetDataSafely<bool>(json, "nsfw");
		last_message_id = GetSnowflakeSafely(json, "last_message_id");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		// recipients = json["recipients"]...;
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetSnowflakeSafely(json, "owner_id");
		application_id = GetSnowflakeSafely(json, "application_id");
		category_id = GetSnowflakeSafely(json, "parent_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	Channel::Channel(nlohmann::json json, snowflake guild_id) : discord::DiscordObject(discord::ToSnowflake(json["id"])), guild_id(guild_id) {
		id = ToSnowflake(json["id"]);
		type = json["type"].get<int>();
		guild_id = GetSnowflakeSafely(json, "guild_id");
		position = GetDataSafely<int>(json, "position");
		// permission_overwrites?
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		nsfw = GetDataSafely<bool>(json, "nsfw");
		last_message_id = GetSnowflakeSafely(json, "last_message_id");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		// recipients = json["recipients"]...;
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetSnowflakeSafely(json, "owner_id");
		application_id = GetSnowflakeSafely(json, "application_id");
		category_id = GetSnowflakeSafely(json, "parent_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	discord::Message Channel::Send(std::string text, bool tts) {
		std::string raw_text = "{\"content\":\"" + text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
		cpr::Body body = cpr::Body(raw_text);

		nlohmann::json test = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), {
			{ "Authorization", Format("Bot %", discord::globals::bot_instance->token) },
			{ "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" },
			{ "Content-Type", "application/json" }
			}, { }, body);

		return discord::Message();
	}

	discord::Channel Channel::Modify(ModifyRequest modify_request) {
		cpr::Header headers = DefaultHeaders({ {"Content-Type", "application/json" } });
		std::string field;
		switch (modify_request.key) {
		case ModifyChannelValue::NAME:
			field = "name";
			break;
		case ModifyChannelValue::POSITION:
			field = "position";
			break;
		case ModifyChannelValue::TOPIC:
			field = "topic";
			break;
		case ModifyChannelValue::NSFW:
			field = "nsfw";
			break;
		case ModifyChannelValue::RATE_LIMIT:
			field = "rate_limit_per_user";
			break;
		case ModifyChannelValue::BITRATE:
			field = "bitrate";
			break;
		case ModifyChannelValue::USER_LIMIT:
			field = "user_limit";
			break;
		case ModifyChannelValue::PERMISSION_OVERWRITES:
			field = "permission_overwrites";
			break;
		case ModifyChannelValue::PARENT_ID:
			field = "parent_id";
			break;
		}

		cpr::Body body = cpr::Body("{\"" + field + "\": \"" + modify_request.value + "\"}");
		nlohmann::json result = SendPatchRequest(Endpoint("/channels/" + std::to_string(id)), headers, body);
		
		*this = discord::Channel(result);
		return *this;
	}

	discord::Channel Channel::Delete() {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders());
		*this = discord::Channel();
		return *this;
	}

	discord::Message Channel::FindMessage(snowflake message_id) {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/messages/" + std::to_string(message_id)), DefaultHeaders(), {}, {});
		return discord::Message(result);
	}
}