#include "channel.h"
#include "utils.h"
#include "client.h"

namespace discpp {
	Channel::Channel(snowflake id) : discpp::DiscordObject(id) {
		/**
		 * @brief Constructs a discpp::Channel object from the id.
		 *
		 * ```cpp
		 *      discpp::Channel channel(674023471063498772);
		 * ```
		 *
		 * @param[in] id The channel id
		 *
		 * @return discpp::Channel, this is a constructor.
		 */
		std::unordered_map<snowflake, Channel>::iterator it = discpp::globals::client_instance->channels.find(id);
		if (it != discpp::globals::client_instance->channels.end()) {
			*this = it->second;
		}
	}

	Channel::Channel(nlohmann::json json) {
		/**
		 * @brief Constructs a discpp::Channel object from json.
		 *
		 * ```cpp
		 *      discpp::channel channel(json);
		 * ```
		 *
		 * @param[in] json The json data for the channel.
		 *
		 * @return discpp::Channel, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		type = GetDataSafely<ChannelType>(json, "type");
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		last_message_id = GetDataSafely<snowflake>(json, "last_message_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	discpp::Message Channel::Send(std::string text, bool tts, discpp::EmbedBuilder* embed, std::vector<File> files) {
		/**
		 * @brief Send a message in this channel.
		 *
		 * ```cpp
	     *      ctx.Send("Hello, I'm a bot!"); // Sending text
	     *      ctx.Send("", false, embed); // Sending an embed
	     *      ctx.Send("Command output was too large to fit in an embed.", false, nullptr, { file }); // Sending files
		 * ```
		 *
		 * @param[in] text The text that goes along with the embed.
		 * @param[in] tts Should it be a text to speech message?
		 * @param[in] embed Embed to send
		 * @param[in] files Files to send
		 *
		 * @return discpp::Message
		 */

		std::string escaped_text = EscapeString(text);
		nlohmann::json message_json = nlohmann::json::parse("{\"content\":\"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");

		if (escaped_text.size() >= 2000) {
			// Write message to file
			std::ofstream message("message.txt", std::ios::out | std::ios::binary);
			message << message_json["content"];
			message.close();

			// Ensure the file will be deleted even if it runs into an exception sending the file.
			discpp::Message sent_message;
			try {
				// Send the message
				std::vector<discpp::File> files;
				files.push_back({ "message.txt", "message.txt" });
				sent_message = Send("Message was too large to fit in 2000 characters", tts, nullptr, files);

				// Delete the temporary message file
				remove("message.txt");
			}
			catch (std::exception e) {
				// Delete the temporary message file
				remove("message.txt");

				throw std::exception(e);
			}

			return sent_message;
		}

		cpr::Body body;
		if (embed != nullptr) {
			body = cpr::Body("{\"embed\": " + embed->ToJson().dump() + ((!text.empty()) ? ", \"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") : "") + "}");
		} else if (!files.empty()) {
			cpr::Multipart multipart_data{};

			for (int i = 0; i < files.size(); i++) {
				multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
			}

			multipart_data.parts.emplace_back("payload_json", "{\"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "\"}");

			WaitForRateLimits(id, RateLimitBucketType::CHANNEL);
			cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/channels/" + id + "/messages") }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
			HandleRateLimits(response.header, id, RateLimitBucketType::CHANNEL);

			return discpp::Message(nlohmann::json::parse(response.text));
		}
		else {
			body = cpr::Body(message_json.dump());
		}
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/messages"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Message(result);
	}

	std::string ChannelPropertyToString(ChannelProperty prop) {
        switch (prop) {
            case ChannelProperty::NAME:
                return "name";
            case ChannelProperty::POSITION:
                return "position";
            case ChannelProperty::TOPIC:
                return "topic";
            case ChannelProperty::NSFW:
                return "nsfw";
            case ChannelProperty::RATE_LIMIT:
                return "rate_limit_per_user";
            case ChannelProperty::BITRATE:
                return "bitrate";
            case ChannelProperty::USER_LIMIT:
                return "user_limit";
            case ChannelProperty::PERMISSION_OVERWRITES:
                return "permission_overwrites";
            case ChannelProperty::PARENT_ID:
                return "parent_id";
        }
	}

    // Helper type for the visitor
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	discpp::Channel Channel::Modify(ModifyRequests& modify_requests) {
		/**
		 * @brief Modify the channel.
		 *
		 * Use discpp::ModifyRequest to modify a field of the channel.
		 *
		 * ```cpp
		 *		// Change the name of the channel to "Test"
		 *		discpp::ModifyRequests request(discpp::ChannelProperty::NAME, "Test");
		 *      channel.Modify(request);
		 * ```
		 *
		 * @param[in] modify_request The field to modify and what to set it to.
		 *
		 * @return discpp::Channel - This method also sets the channel reference to the returned channel.
		 */

		cpr::Header headers = DefaultHeaders({ {"Content-Type", "application/json" } });
		std::string field;
        nlohmann::json j_body = {};
        for (auto request : modify_requests.requests) {
            std::variant<std::string, int, bool> variant = request.second;
            std::visit(overloaded {
                [&](bool b) { j_body[ChannelPropertyToString(request.first)] = (b) ? "true" : "false"; },
                [&](int i) { j_body[ChannelPropertyToString(request.first)] = i; },
                [&](const std::string& str) { j_body[ChannelPropertyToString(request.first)] = str; }
            }, variant);
        }

		cpr::Body body(j_body.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/channels/" + id), headers, id, RateLimitBucketType::CHANNEL, body);
		
		*this = discpp::Channel(result);
		return *this;
	}

	discpp::Channel Channel::Delete() {
		/**
		 * @brief Delete this channel.
		 *
		 * ```cpp
		 *      channel.Delete();
		 * ```
		 *
		 * @return discpp::Channel - Returns a default channel object
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		*this = discpp::Channel();
		return *this;
	}

	std::vector<discpp::Message> Channel::GetChannelMessages(int amount, GetChannelsMessagesMethod get_method) {
		/**
		 * @brief Get channel's messages depending on the given method.
		 *
		 * ```cpp
		 *      std::vector<discpp::Message> messages = channel.GetChannelMessages(50);
		 * ```
		 *
		 * @param[in] amount The amount of the messages to get unless the method is not "limit".
		 * @param[in] get_method The method of how to get the messages.
		 *
		 * @return std::vector<discpp::Message>
		 */
		
		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/messages"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

		std::vector<discpp::Message> messages;

		for (nlohmann::json message : result) {
			messages.push_back(discpp::Message(message));
		}

		return messages;
	}

	discpp::Message Channel::FindMessage(snowflake message_id) {
		/**
		 * @brief Get a message from the channel from the id.
		 *
		 * ```cpp
		 *      channel.FindMessage(685199299042476075);
		 * ```
		 *
		 * @param[in] message_id The message id.
		 *
		 * @return discpp::Message
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/messages/" + message_id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		return discpp::Message(result);
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

	std::vector<discpp::Message> Channel::GetPinnedMessages() {
		/**
		 * @brief Get all messages pinned to the channel.
		 *
		 * ```cpp
		 *      channel.GetPinnedMessages();
		 * ```
		 *
		 * @return std::vector<discpp::Message>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id = "/pins"), DefaultHeaders(), {}, {});
		
		std::vector<discpp::Message> messages;
		for (auto message : result) {
			messages.push_back(discpp::Message(message));
		}

		return messages;
	}

	GuildChannel::GuildChannel(nlohmann::json json) : Channel(json) {
		/**
		 * @brief Constructs a discpp::GuildChannel object from json.
		 *
		 * ```cpp
		 *      discpp::GuildChannel GuildChannel(json);
		 * ```
		 *
		 * @param[in] json The json data for the guild channel.
		 *
		 * @return discpp::GuildChannel, this is a constructor.
		 */

		guild_id = GetDataSafely<snowflake>(json, "guild_id");
		position = GetDataSafely<int>(json, "position");
		if (json.contains("permission_overwrites")) {
			for (auto permission_overwrite : json["permission_overwrites"]) {
				permissions.push_back(discpp::Permissions(permission_overwrite));
			}
		}
		nsfw = GetDataSafely<bool>(json, "nsfw");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		category_id = GetDataSafely<snowflake>(json, "parent_id");
	}

	GuildChannel::GuildChannel(snowflake id, snowflake guild_id) : Channel(id) {
		/**
		 * @brief Constructs a discpp::GuildChannel from id and guild
		 *
		 * ```cpp
		 *      discpp::GuildChannel GuildChannel(json);
		 * ```
		 *
		 * @param[in] id the id of the channel
		 * @param[in] guild_id the guild id
		 *
		 * @return discpp::GuildChannel, this is a constructor.
		 */

		discpp::Guild guild(guild_id);
		std::unordered_map<snowflake, GuildChannel>::iterator channels = guild.channels.find(id);
		if (channels != guild.channels.end()) {
			*this = channels->second;
		}
	}

	void GuildChannel::BulkDeleteMessage(std::vector<snowflake>& messages) {
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

	std::vector<discpp::GuildInvite> GuildChannel::GetInvites() {
		/**
		 * @brief Get all the channel invites.
		 *
		 * ```cpp
		 *     std::vector<discpp::GuildInvite> invites = channel.GetInvites();
		 * ```
		 *
		 * @return std::vector<discpp::GuildInvite>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/channels/" + id + "/invites"), DefaultHeaders(), {}, {});
		std::vector<discpp::GuildInvite> invites;
		for (auto invite : result) {
			invites.push_back(discpp::GuildInvite(invite));
		}

		return invites;
	}

	discpp::GuildInvite GuildChannel::CreateInvite(int max_age, int max_uses, bool temporary, bool unique) {
		/**
		 * @brief Create an invite for the channel.
		 *
		 * ```cpp
		 *      discpp::GuildInvite invite = channel.CreateInvite(86400, 5, true, true);
		 * ```
		 *
		 * @param[in] max_age How long the invite will last for.
		 * @param[in] max_uses Max uses of the invite.
		 * @param[in] temporary Whether this invite only grants temporary membership.
		 * @param[in] If true, dont try to reuse similar invites.
		 *
		 * @return discpp::GuildInvite
		 */

		cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/channels/" + id + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
		discpp::GuildInvite invite(result);

		return invite;
	}

	void GuildChannel::EditPermissions(discpp::Permissions& permissions) {
		/**
		 * @brief Edit permission overwrites for this channel.
		 *
		 * ```cpp
		 *      channel.EditPermissions(permissions);
		 * ```
		 *
		 * @param[in] permissions The permissions that the channels permission overwrites will be set to.
		 *
		 * @return void
		 */

		std::string s_type = (permissions.permission_type == PermissionType::MEMBER) ? "member" : "role";

		nlohmann::json j_body = {
				{"allow" , permissions.allow_perms.value},
				{"deny", permissions.deny_perms.value},
				{"type", s_type}
		};

		nlohmann::json result = SendPutRequest(Endpoint("/channels/" + id + "/permissions/" + permissions.role_user_id), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, cpr::Body(j_body.dump()));
	}

	void GuildChannel::DeletePermission(discpp::Permissions& permissions) {
		/**
		 * @brief Remove permission overwrites for this channel.
		 *
		 * ```cpp
		 *      channel.DeletePermission(permissions);
		 * ```
		 *
		 * @param[in] permissions The permissions that will be removed
		 *
		 * @return void
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + id + "/permissions/" + permissions.role_user_id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	DMChannel::DMChannel(nlohmann::json json) : Channel(json) {
		if (json.contains("recipients")) {
			for (auto recipient : json["recipients"]) {
				recipients.push_back(discpp::User(recipient));
			}
		}
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetDataSafely<snowflake>(json, "owner_id");
		application_id = GetDataSafely<snowflake>(json, "application_id");
	}

	void DMChannel::GroupDMAddRecipient(discpp::User& user) {
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

	void DMChannel::GroupDMRemoveRecipient(discpp::User& user) {
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