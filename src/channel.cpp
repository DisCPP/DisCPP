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
		auto it = discpp::globals::client_instance->channels.find(id);
		if (it != discpp::globals::client_instance->channels.end()) {
			*this = *it->second;
		}
	}

	Channel::Channel(rapidjson::Document& json) {
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
		id = SnowflakeFromString(json["id"].GetString());
		type = static_cast<ChannelType>(json["type"].GetInt());
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		last_message_id = GetIDSafely(json, "last_message_id");
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
		rapidjson::Document message_json;
		std::string message_json_str = "{\"content\":\"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
		message_json.Parse(message_json_str);

		if (escaped_text.size() >= 2000) {
			// Write message to file
			std::ofstream message("message.txt", std::ios::out | std::ios::binary);
			message << message_json["content"].GetString();
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
			} catch (std::exception e) {
				// Delete the temporary message file
				remove("message.txt");

				throw std::exception(e);
			}

			return sent_message;
		}

		cpr::Body body;
		if (embed != nullptr) {
		    rapidjson::Document embed_json = embed->ToJson();
			body = cpr::Body("{\"embed\": " + DumpJson(embed_json) + ((!text.empty()) ? ", \"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") : "") + "}");
		} else if (!files.empty()) {
			cpr::Multipart multipart_data{};

			for (int i = 0; i < files.size(); i++) {
				multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
			}

			multipart_data.parts.emplace_back("payload_json", "{\"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "\"}");

			WaitForRateLimits(id, RateLimitBucketType::CHANNEL);
			cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/channels/" + std::to_string(id) + "/messages") }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
			HandleRateLimits(response.header, id, RateLimitBucketType::CHANNEL);

			rapidjson::Document result_json(rapidjson::kObjectType);
			result_json.Parse(response.text);
			return discpp::Message(result_json);
		} else {
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			message_json.Accept(writer);
			std::string json_payload = buffer.GetString();
			body = cpr::Body(json_payload);
		}
		rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

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

        rapidjson::Document j_body(rapidjson::kObjectType);
        for (auto request : modify_requests.requests) {
            std::variant<std::string, int, bool> variant = request.second;
            std::visit(overloaded {
                [&](bool b) { j_body[ChannelPropertyToString(request.first)].SetBool(b); },
                [&](int i) { j_body[ChannelPropertyToString(request.first)].SetInt(i); },
                [&](const std::string& str) { j_body[ChannelPropertyToString(request.first)].SetString(rapidjson::StringRef(str)); }
            }, variant);
        }

		cpr::Body body(DumpJson(j_body));
		rapidjson::Document result = SendPatchRequest(Endpoint("/channels/" + std::to_string(id)), headers, id, RateLimitBucketType::CHANNEL, body);
		
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

		rapidjson::Document result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
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
		
		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

		std::vector<discpp::Message> messages;

		for (auto& message : result.GetArray()) {
			rapidjson::Document message_json;
			message_json.CopyFrom(message, message_json.GetAllocator());
			messages.push_back(discpp::Message(message_json));
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

		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/messages/" + std::to_string(message_id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		return discpp::Message(result);
	}

	inline void Channel::TriggerTypingIndicator() {
		/**
		 * @brief Triggers a typing indicator.
		 *
		 * ```cpp
		 *      channel.TriggerTypingIndicator();
		 * ```
		 *
		 * @return void
		 */

		rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/typing"), DefaultHeaders(), {}, {});
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

		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) = "/pins"), DefaultHeaders(), {}, {});
		
		std::vector<discpp::Message> messages;
		for (auto& message : result.GetArray()) {
			rapidjson::Document message_json;
			message_json.CopyFrom(message, message_json.GetAllocator());
			messages.push_back(discpp::Message(message_json));
		}

		return messages;
	}

	GuildChannel::GuildChannel(rapidjson::Document& json) : Channel(json) {
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

		guild_id = GetIDSafely(json, "guild_id");
		position = GetDataSafely<int>(json, "position");
		if (ContainsNotNull(json, "permission_overwrites")) {
			for (auto& permission_overwrite : json["permission_overwrites"].GetArray()) {
				rapidjson::Document permission_json;
				permission_json.CopyFrom(permission_overwrite, permission_json.GetAllocator());

				permissions.push_back(discpp::Permissions(permission_json));
			}
		}
		nsfw = GetDataSafely<bool>(json, "nsfw");
		bitrate = GetDataSafely<int>(json, "bitrate");
		user_limit = GetDataSafely<int>(json, "user_limit");
		rate_limit_per_user = GetDataSafely<int>(json, "rate_limit_per_user");
		category_id = GetIDSafely(json, "parent_id");
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

		auto channels = guild.channels.find(id);
		if (channels != guild.channels.end()) {
			*this = *channels->second;
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(id) + "/messages/bulk-delete");

		std::string combined_message = "";
		for (snowflake message : messages) {
			if (message == messages[0]) {
				combined_message += "\"" + std::to_string(message) + "\"";
			} else {
				combined_message += ", \"" + std::to_string(message) + "\"";
			}
		}

		cpr::Body body("{\"messages\": [" + combined_message + "]}");
		rapidjson::Document result = SendPostRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
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

		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});
		std::vector<discpp::GuildInvite> invites;
		for (auto& invite : result.GetArray()) {
			rapidjson::Document invite_json;
			invite_json.CopyFrom(invite, invite_json.GetAllocator());
			invites.push_back(discpp::GuildInvite(invite_json));
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
		rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
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

		rapidjson::Document permission_json;
		permission_json.SetObject();
		rapidjson::Document::AllocatorType& permission_allocator = permission_json.GetAllocator();
		permission_json.AddMember("allow", permissions.allow_perms.value, permission_allocator);
		permission_json.AddMember("deny", permissions.deny_perms.value, permission_allocator);
		permission_json.AddMember("type", s_type, permission_allocator);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		permission_json.Accept(writer);
		std::string json_payload = buffer.GetString();

		SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/permissions/" + std::to_string(permissions.role_user_id)), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, cpr::Body(json_payload));
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

		SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/permissions/" + std::to_string(permissions.role_user_id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	DMChannel::DMChannel(rapidjson::Document& json) : Channel(json) {
		rapidjson::Value::ConstMemberIterator itr = json.FindMember("recipients");
		if (itr != json.MemberEnd()) {
			for (auto& recipient : json["recipients"].GetArray()) {
				rapidjson::Document user_json;
				user_json.CopyFrom(recipient, user_json.GetAllocator());
				recipients.push_back(discpp::User(user_json));
			}
		}
		icon = GetDataSafely<std::string>(json, "icon");
		owner_id = GetIDSafely(json, "owner_id");
		application_id = GetIDSafely(json, "application_id");
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

		SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
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

		SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}