#include "channel.h"
#include "utils.h"
#include "client.h"

namespace discpp {
	Channel::Channel(const snowflake& id) : discpp::DiscordObject(id) {
		*this = globals::client_instance->GetChannel(id);
	}

	Channel::Channel(rapidjson::Document& json) {
		id = SnowflakeFromString(json["id"].GetString());
		type = static_cast<ChannelType>(json["type"].GetInt());
		name = GetDataSafely<std::string>(json, "name");
		topic = GetDataSafely<std::string>(json, "topic");
		last_message_id = GetIDSafely(json, "last_message_id");
		last_pin_timestamp = GetDataSafely<std::string>(json, "last_pin_timestamp");
	}

	discpp::Message Channel::Send(const std::string& text, const bool& tts, discpp::EmbedBuilder* embed, std::vector<File> files) {
		std::string escaped_text = EscapeString(text);

		// Send a file filled with message contents if the message is more than 2000 characters.
		if (escaped_text.size() >= 2000) {
			// Write message to file
			std::ofstream message("message.txt", std::ios::out | std::ios::binary);
			message << escaped_text;
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

        rapidjson::Document message_json;
        std::string message_json_str = "{\"content\":\"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
        message_json.Parse(message_json_str);

		cpr::Body body;
		if (embed != nullptr) { // Set the HTTP payload to an embed.
		    rapidjson::Document embed_json = embed->ToJson();
			body = cpr::Body("{\"embed\": " + DumpJson(embed_json) + ((!text.empty()) ? ", \"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") : "") + "}");
		} else if (!files.empty()) { // Send files.
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
			body = cpr::Body(DumpJson(message_json));
		}

		rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		return discpp::Message(result);
	}

	std::string ChannelPropertyToString(ChannelProperty prop) {
        std::unordered_map<ChannelProperty, std::string> prop_str_map = {
                {ChannelProperty::NAME, "name"}, {ChannelProperty::POSITION, "position"},
                {ChannelProperty::TOPIC, "topic"}, {ChannelProperty::NSFW, "nsfw"},
                {ChannelProperty::RATE_LIMIT, "rate_limit_per_user"}, {ChannelProperty::BITRATE, "bitrate"},
                {ChannelProperty::USER_LIMIT, "user_limit"}, {ChannelProperty::PERMISSION_OVERWRITES, "permission_overwrites"},
                {ChannelProperty::PARENT_ID, "parent_id"}
        };
        return prop_str_map[prop];
	}

    // Helper type for the visitor
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	discpp::Channel Channel::Modify(ModifyRequests& modify_requests) {
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
		rapidjson::Document result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		*this = discpp::Channel();
		return *this;
	}

	std::vector<discpp::Message> Channel::GetChannelMessages(int amount, GetChannelsMessagesMethod get_method) {
		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

		std::vector<discpp::Message> messages;

		for (auto& message : result.GetArray()) {
			rapidjson::Document message_json;
			message_json.CopyFrom(message, message_json.GetAllocator());
			messages.push_back(discpp::Message(message_json));
		}

		return messages;
	}

	discpp::Message Channel::FindMessage(const snowflake& message_id) {
		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/messages/" + std::to_string(message_id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		return discpp::Message(result);
	}

	void Channel::TriggerTypingIndicator() {
		rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/typing"), DefaultHeaders(), {}, {});
	}

	std::vector<discpp::Message> Channel::GetPinnedMessages() {
        rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) = "/pins"),
                                                    DefaultHeaders(), {}, {});

        std::vector<discpp::Message> messages;
        for (auto &message : result.GetArray()) {
            rapidjson::Document message_json;
            message_json.CopyFrom(message, message_json.GetAllocator());
            messages.push_back(discpp::Message(message_json));
        }

        return messages;
    }

    discpp::Channel Channel::RequestChannel(discpp::snowflake id) {
        rapidjson::Document channel = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
        return discpp::Channel(channel);
    }

    GuildChannel::GuildChannel(rapidjson::Document& json) : Channel(json) {
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

	GuildChannel::GuildChannel(const snowflake& id, const snowflake& guild_id) {
		discpp::Guild guild(guild_id);

		auto channels = guild.channels.find(id);
		if (channels != guild.channels.end()) {
			*this = channels->second;
		}
	}

	void GuildChannel::BulkDeleteMessage(const std::vector<snowflake>& messages) {
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

    void GuildChannel::DeletePermission(const discpp::Permissions& permissions) {
        SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/permissions/" + std::to_string(permissions.role_user_id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
    }

    void GuildChannel::EditPermissions(const discpp::Permissions& permissions) {
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

    std::shared_ptr<discpp::Guild> GuildChannel::GetGuild() {
        std::shared_ptr<Guild> tmp = globals::client_instance->GetGuild(guild_id);
        return tmp;
    }

    discpp::GuildInvite GuildChannel::CreateInvite(const int& max_age, const int& max_uses, const bool& temporary, const bool& unique) {
        cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
        rapidjson::Document result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
        discpp::GuildInvite invite(result);

        return invite;
    }

	std::vector<discpp::GuildInvite> GuildChannel::GetInvites() {
		rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});
		std::vector<discpp::GuildInvite> invites;
		for (auto& invite : result.GetArray()) {
			rapidjson::Document invite_json;
			invite_json.CopyFrom(invite, invite_json.GetAllocator());
			invites.push_back(discpp::GuildInvite(invite_json));
		}

		return invites;
	}

	std::unordered_map<discpp::snowflake, discpp::GuildChannel> CategoryChannel::GetChildren() {
	    std::unordered_map<discpp::snowflake, discpp::GuildChannel> tmp;
	    for (auto const chnl : this->GetGuild()->channels) {
	        if (chnl.second.category_id == this->id) {
                tmp.insert({chnl.first, chnl.second});
	        } else {
	            continue;
	        }
	    }
	    return tmp;
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

    DMChannel::DMChannel(const snowflake& id) {
        auto private_channel = globals::client_instance->private_channels.find(id);
        if (private_channel != globals::client_instance->private_channels.end()) {
            *this = private_channel->second;
        }
    }

	void DMChannel::GroupDMAddRecipient(const discpp::User& user) {
		SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void DMChannel::GroupDMRemoveRecipient(const discpp::User& user) {
		SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}