#include "channel.h"
#include "utils.h"
#include "bot.h"
#include "user.h"
#include "guild.h"

#include <cpprest/http_client.h>

namespace discord {
	Channel::Channel(snowflake id) : discord::DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Channel object from the id.
		 *
		 * ```cpp
		 *      discord::Channel channel(674023471063498772);
		 * ```
		 *
		 * @param[in] id The channel id
		 *
		 * @return discord::Channel, this is a constructor.
		 */

		auto channel = std::find_if(discord::globals::bot_instance->channels.begin(), discord::globals::bot_instance->channels.end(), [id](discord::Channel a) { return id == a.id; });
		
		if (channel != discord::globals::bot_instance->channels.end()) {
			*this = *channel;
		}
	}

	Channel::Channel(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Channel object from json.
		 *
		 * ```cpp
		 *      discord::channel channel(json);
		 * ```
		 *
		 * @param[in] json The json data for the channel.
		 *
		 * @return discord::Channel, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		type = GetDataSafely<int>(json, "type");
		if (json.contains("guild_id")) { // Can't use GetDataSafely so it doesn't over write the other constructor.
			guild_id = json["guild_id"].get<snowflake>();
		}
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
		/**
		 * @brief Constructs a discord::Channel object from json with a guild id.
		 *
		 * ```cpp
		 *      discord::Channel channel(json, 583251190591258624);
		 * ```
		 *
		 * @param[in] json The json data for the channel.
		 * @param[in] guild_id The guild id for this channel.
		 *
		 * @return discord::Bot, this is a constructor.
		 */

		this->guild_id = guild_id;
	}

	discord::Message Channel::Send(std::string text, bool tts) {
		/**
		 * @brief Send a message in this channel.
		 *
		 * If the message is over 2000 characters, then it will write the message to a temporary file and send that. It will then delete the file after its sent.
		 *
		 * ```cpp
		 *      channel.Send("Test");
		 * ```
		 *
		 * @param[in] text The message to send.
		 * @param[in] tts Should it be a text to speech message?
		 *
		 * @return discord::Message
		 */

		std::string escaped_text = EscapeString(text);
		nlohmann::json message_json = nlohmann::json::parse("{\"content\":\"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");

		if (escaped_text.size() >= 2000) {
			// Write message to file
			std::ofstream message("message.txt", std::ios::out | std::ios::binary);
			message << message_json["content"];
			message.close();

			// Ensure the file will be deleted even if it runs into an exception sending the file.
			discord::Message sent_message;
			try {
				// Send the message
				std::vector<discord::File> files;
				files.push_back({ "message.txt", "message.txt" });
				sent_message = Send(files, "Message was too large to fit in 2000 characters");

				// Delete the temporary message file
				remove("message.txt");
			} catch (std::exception e) {
				// Delete the temporary message file
				remove("message.txt");

				throw std::exception(e);
			}

			return sent_message;
		}

		cpr::Body body = cpr::Body(message_json.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/messages"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		return discord::Message(result);
	}

	discord::Message Channel::Send(discord::EmbedBuilder embed, std::string text) {
		/**
		 * @brief Send an embed in this channel.
		 *
		 * ```cpp
		 *      channel.Send(embed);
		 * ```
		 *
		 * @param[in] embed The embed to send.
		 * @param[in] text The text that goes along with the embed.
		 *
		 * @return discord::Message
		 */

		cpr::Body body = cpr::Body("{\"embed\": " + embed.ToJson().dump() + ((!text.empty()) ? ", \"content\": \"" + EscapeString(text) + "\"" : "") + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/messages"), DefaultHeaders({ {"Content-Type", "application/json"} }), id, RateLimitBucketType::CHANNEL, body);

		return discord::Message(result);
	}

	discord::Message discord::Channel::Send(std::vector<File> files, std::string text) {
		/**
		 * @brief Send a file(s) through the channel.
		 *
		 * ```cpp
		 *      channel.Send({ discord::File{file_1} });
		 * ```
		 *
		 * @param[in] files The file(s) to send.
		 * @param[in] text The text that goes along with the files.
		 *
		 * @return discord::Message
		 */

		cpr::Multipart multipart_data{};

		for (int i = 0; i < files.size(); i++) {
			multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
		}

		multipart_data.parts.emplace_back("payload_json", "{\"content\": \"" + EscapeString(text) + "\"}");

		WaitForRateLimits(id, RateLimitBucketType::CHANNEL);
		cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/channels/" + id + "/messages") }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
		HandleRateLimits(response.header, id, RateLimitBucketType::CHANNEL);

		return discord::Message(nlohmann::json::parse(response.text));
	}

	discord::Channel Channel::Modify(ModifyRequest modify_request) {
		/**
		 * @brief Modify the channel.
		 *
		 * Use discord::ModifyRequest to modify a field of the channel.
		 *
		 * ```cpp
		 *		// Change the name of the channel to "Test"
		 *		discord::ModifyRequest request(discord::ModifyChannelValue::NAME, "Test");
		 *      channel.Modify(request);
		 * ```
		 *
		 * @param[in] modify_request The field to modify and what to set it to.
		 *
		 * @return discord::Channel - This method also sets the channel reference to the returned channel.
		 */

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
		nlohmann::json result = SendPatchRequest(Endpoint("/channels/" + id), headers, id, RateLimitBucketType::CHANNEL, body);
		
		*this = discord::Channel(result);
		return *this;
	}

	discord::Channel Channel::Delete() {
		/**
		 * @brief Delete this channel.
		 *
		 * ```cpp
		 *      channel.Delete();
		 * ```
		 *
		 * @return discord::Channel - Returns a default channel object
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		*this = discord::Channel();
		return *this;
	}

	std::vector<discord::Message> Channel::GetChannelMessages(int amount, GetChannelsMessagesMethod get_method) {
		/**
		 * @brief Get channel's messages depending on the given method.
		 *
		 * ```cpp
		 *      std::vector<discord::Message> messages = channel.GetChannelMessages(50);
		 * ```
		 *
		 * @param[in] amount The amount of the messages to get unless the method is not "limit".
		 * @param[in] get_method The method of how to get the messages.
		 *
		 * @return std::vector<discord::Message>
		 */
		
		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/messages"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

		std::vector<discord::Message> messages;

		for (nlohmann::json message : result) {
			messages.push_back(discord::Message(message));
		}

		return messages;
	}

	discord::Message Channel::FindMessage(snowflake message_id) {
		/**
		 * @brief Get a message from the channel from the id.
		 *
		 * ```cpp
		 *      channel.FindMessage(685199299042476075);
		 * ```
		 *
		 * @param[in] message_id The message id.
		 *
		 * @return discord::Message
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/messages/" + message_id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		return discord::Message(result);
	}

	void Channel::BulkDeleteMessage(std::vector<snowflake> messages) {
		/**
		 * @brief Delete several messages (2-100).
		 *
		 * ```cpp
		 *      channel.BulkDeleteMessage({message_a, message_b, message_c});
		 * ```
		 *
		 * @param[in] messages The messages to delete.
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + id + "/messages/bulk-delete");

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
		/**
		 * @brief Get all the channel invites.
		 *
		 * ```cpp
		 *     std::vector<discord::GuildInvite> invites = channel.GetInvites();
		 * ```
		 *
		 * @return std::vector<discord::GuidlInvite>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/invites"), DefaultHeaders(), {}, {});
		std::vector<discord::GuildInvite> invites;
		for (auto invite : result) {
			invites.push_back(discord::GuildInvite(invite));
		}

		return invites;
	}

	discord::GuildInvite Channel::CreateInvite(int max_age, int max_uses, bool temporary, bool unique) {
		/**
		 * @brief Create an invite for the channel.
		 *
		 * ```cpp
		 *      discord::GuildInvite invite = channel.CreateInvite(86400, 5, true, true);
		 * ```
		 *
		 * @param[in] max_age How long the invite will last for.
		 * @param[in] max_uses Max uses of the invite.
		 * @param[in] temporary Whether this invite only grants temporary membership.
		 * @param[in] If ture, dont try to reuse similar invites.
		 *
		 * @return discord::GuildInvite
		 */

		cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
		discord::GuildInvite invite(result);

		return invite;
	}

	void Channel::TriggerTypingIndicator() {
		/**
		 * @brief Triggers a typing indicator.
		 *
		 * ```cpp
		 *      channel.TriggerTypingIndicator();
		 * ```
		 *
		 * @return void
		 */

		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/typing"), DefaultHeaders(), {}, {});
	}

	std::vector<discord::Message> Channel::GetPinnedMessages() {
		/**
		 * @brief Get all messages pinned to the channel.
		 *
		 * ```cpp
		 *      channel.GetPinnedMessages();
		 * ```
		 *
		 * @return std::vector<discord::Message>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id = "/pins"), DefaultHeaders(), {}, {});
		
		std::vector<discord::Message> messages;
		for (auto message : result) {
			messages.push_back(discord::Message(message));
		}

		return messages;
	}

	void Channel::GroupDMAddRecipient(discord::User user) {
		/**
		 * @brief Add a recipient to the group dm.
		 *
		 * This only works if the channel is a group dm.
		 *
		 * ```cpp
		 *      channel.GroupDMAddRecipient(user);
		 * ```
		 *
		 * @return void
		 */

		nlohmann::json result = SendPutRequest(Endpoint("/channels/" + id + "/recipients/" + user.id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Channel::GroupDMRemoveRecipient(discord::User user) {
		/**
		 * @brief Remove a recipient from the group dm.
		 *
		 * ```cpp
		 *      channel.GroupDMRemoveRecipient(user);
		 * ```
		 *
		 * @param[in] User The user to remove.
		 *
		 * @return void
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + id + "/recipients/" + user.id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}