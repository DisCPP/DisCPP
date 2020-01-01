#include "channel.h"
#include "utils.h"
#include "bot.h"
#include "user.h"
#include "guild.h"

#include <cpprest/http_client.h>

namespace discord {
	Channel::Channel(snowflake id) : discord::DiscordObject(id) {
		auto channel = std::find_if(discord::globals::bot_instance->channels.begin(), discord::globals::bot_instance->channels.end(), [id](discord::Channel a) { return id == a.id; });
		
		if (channel != discord::globals::bot_instance->channels.end()) {
			*this = *channel;
		}
	}

	Channel::Channel(nlohmann::json json) : discord::DiscordObject(json["id"].get<snowflake>()) {
		id = json["id"].get<snowflake>();
		type = json["type"].get<int>();
		if (json.contains("guild_id")) { // Can't use GetDataSafely so it doesn't over write the other constructor.
			guild_id = json["guild_id"].get<snowflake>();
		}
		/*if (json.contains("guild_id")) {
			guild = discord::Guild(json["guild_id"].get<snowflake>());
		}*/
		position = GetDataSafely<int>(json, "position");
		if (json.contains("permission_overwrites")) {
			for (auto permission_overwrite : json["permission_overwrites"]) {
				permissions.push_back(discord::Permissions(permission_overwrite));
			}
		}
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		nsfw = GetDataSafely<bool>(json, "nsfw");
		last_message_id = GetDataSafely<snowflake>(json, "last_message_id");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		if (json.contains("recipients")) {
			for (auto recipient : json["recipients"]) {
				recipients.push_back(discord::User(recipient));
			}
		}
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetDataSafely<snowflake>(json, "owner_id");
		application_id = GetDataSafely<snowflake>(json, "application_id");
		category_id = GetDataSafely<snowflake>(json, "parent_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	Channel::Channel(nlohmann::json json, snowflake guild_id) : Channel(json) {
		this->guild_id = guild_id;
	}

	discord::Message Channel::Send(std::string text, bool tts) {
		std::string raw_text = "{\"content\":\"" + text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
		cpr::Body body = cpr::Body(raw_text);
		nlohmann::json result = SendPostRequest(Endpoint("/channels/%/messages", id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		return discord::Message(result);
	}

	discord::Message Channel::Send(discord::EmbedBuilder embed, std::string text) {
		cpr::Body body = cpr::Body(Format("{\"embed\": %%}", embed.ToJson(), ( (!text.empty()) ? Format(", \"content\": \"%\"", text) : "") ));
		nlohmann::json result = SendPostRequest(Endpoint("/channels/%/messages", id), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discord::Message(result);
	}

	discord::Message discord::Channel::Send(std::vector<File> files, std::string text) {
		cpr::Multipart multipart_data{};

		for (int i = 0; i < files.size(); i++) {
			multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
		}

		multipart_data.parts.emplace_back("payload_json", Format("{\"content\": \"%\"}", text));

		WaitForRateLimits(id, RateLimitBucketType::CHANNEL);
		cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/channels/%/messages", id) }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
		HandleRateLimits(response.header, id, RateLimitBucketType::CHANNEL);

		return discord::Message(nlohmann::json::parse(response.text));
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
		nlohmann::json result = SendPatchRequest(Endpoint("/channels/%", id), headers, id, RateLimitBucketType::CHANNEL, body);
		
		*this = discord::Channel(result);
		return *this;
	}

	discord::Channel Channel::Delete() {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/%", id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		*this = discord::Channel();
		return *this;
	}

	discord::Message Channel::FindMessage(snowflake message_id) {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/%/messages/%", id, message_id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		return discord::Message(result);
	}

	void Channel::BulkDeleteMessage(std::vector<snowflake> messages) {
		std::string endpoint = Endpoint("/channels/%/messages/bulk-delete", id);

		std::string combined_message = "";
		for (snowflake message : messages) {
			if (message == messages[0]) {
				combined_message += "\"" + message + "\"";
			}
			else {
				combined_message += ", \"" + message + "\"";
			}
		}

		cpr::Body body("{\"messages\": [" + combined_message + "]}");
		nlohmann::json result = SendPostRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
	}

	std::vector<discord::GuildInvite> Channel::GetInvites() {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/%/invites", id), DefaultHeaders(), {}, {});
		std::vector<discord::GuildInvite> invites;
		for (auto invite : result) {
			invites.push_back(discord::GuildInvite(invite));
		}

		return invites;
	}

	discord::GuildInvite Channel::CreateInvite(int max_age, int max_uses, bool temporary, bool unique) {
		cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/channels/%/invites", id), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
		discord::GuildInvite invite(result);

		return invite;
	}

	void Channel::TriggerTypingIndicator() {
		nlohmann::json result = SendPostRequest(Endpoint("/channels/%/typing", id), DefaultHeaders(), {}, {});
	}

	std::vector<discord::Message> Channel::GetPinnedMessages() {
		nlohmann::json result = SendGetRequest(Endpoint("/channels/%/pins", id), DefaultHeaders(), {}, {});
		
		std::vector<discord::Message> messages;
		for (auto message : result) {
			messages.push_back(discord::Message(message));
		}

		return messages;
	}

	void Channel::GroupDMAddRecipient(discord::User user) {
		nlohmann::json result = SendPutRequest(Endpoint("/channels/%/recipients/%", id, user.id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Channel::GroupDMRemoveRecipient(discord::User user) {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/%/recipients/%", id, user.id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}