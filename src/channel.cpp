#include "channel.h"
#include "utils.h"
#include "bot.h"
#include "invite.h"
#include "user.h"

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
		if (json.contains("permission_overwrites")) {
			for (auto permission_overwrite : json["permission_overwrites"]) {
				permissions.push_back(discord::Permission(permission_overwrite));
			}
		}
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		nsfw = GetDataSafely<bool>(json, "nsfw");
		last_message_id = GetSnowflakeSafely(json, "last_message_id");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		if (json.contains("recipients")) {
			for (auto recipient : json["recipients"]) {
				recipients.push_back(discord::User(recipient));
			}
		}
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetSnowflakeSafely(json, "owner_id");
		application_id = GetSnowflakeSafely(json, "application_id");
		category_id = GetSnowflakeSafely(json, "parent_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	Channel::Channel(nlohmann::json json, snowflake guild_id) : discord::DiscordObject(discord::ToSnowflake(json["id"])), guild_id(guild_id) {
		*this = Channel(json);
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

	void Channel::BulkDeleteMessage(std::vector<snowflake> messages) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(id) + "/messages/bulk-delete");

		std::string combined_message = "";
		for (snowflake message : messages) {
			if (message == messages[0]) {
				combined_message += "\"" + std::to_string(message) + "\"";
			}
			else {
				combined_message += ", \"" + std::to_string(message) + "\"";
			}
		}

		cpr::Body body("{\"messages\": [" + combined_message + "]}");
		nlohmann::json result = SendPostRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), {}, body);
	}

	std::vector<discord::Invite> Channel::GetInvites() {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});
		std::vector<discord::Invite> invites;
		for (auto invite : result) {
			invites.push_back(discord::Invite(invite));
		}

		return invites;
	}

	discord::Invite Channel::CreateInvite(int max_age, int max_uses, bool temporary, bool unique) {
		cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), {}, body);
		discord::Invite invite(result);

		return invite;
	}

	void Channel::TriggerTypingIndicator() {
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/typing"), DefaultHeaders(), {}, {});
	}

	std::vector<discord::Message> Channel::GetPinnedMessages() {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/pins"), DefaultHeaders(), {}, {});
		
		std::vector<discord::Message> messages;
		for (auto message : result) {
			messages.push_back(discord::Message(message));
		}

		return messages;
	}

	void Channel::GroupDMAddRecipient(discord::User user) {
		nlohmann::json result = SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), {});
	}

	void Channel::GroupDMRemoveRecipient(discord::User user) {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders());
	}
}