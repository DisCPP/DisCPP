#include "channel.h"
#include "utils.h"
#include "client.h"
#include "message.h"
#include "log.h"
#include "guild.h"
#include "exceptions.h"

#ifdef RAPIDJSON_BACKEND
#include <rapidjson/writer.h>
#elif SIMDJSON_BACKEND

#endif

namespace discpp {
	Channel::Channel(const Snowflake& id, bool can_request) : discpp::DiscordObject(id) {
		*this = globals::client_instance->cache.GetChannel(id, can_request);
	}

	Channel::Channel(const discpp::JsonObject& json) {
	    id = SnowflakeFromString(json["id"].GetString());
		type = static_cast<ChannelType>(json["type"].GetInt());
		name = json.Get<std::string>("name");
		topic = json.Get<std::string>("topic");
		last_message_id = json.GetIDSafely("last_message_id");
		if (json.ContainsNotNull("last_pin_timestamp")) last_pin_timestamp = TimeFromDiscord(json["last_pin_timestamp"].GetString());
        guild_id = json.GetIDSafely("guild_id");
        position = json.Get<int>("position");

        if (json.ContainsNotNull("permission_overwrites")) {
            json["permission_overwrites"].IterateThrough([&] (const discpp::JsonObject& permission_json)->bool {
                permissions.emplace_back(permission_json);
                return true;
            });
        }

        nsfw = json.Get<bool>("nsfw");
        bitrate = json.Get<int>("bitrate");
        user_limit = json.Get<int>("user_limit");
        rate_limit_per_user = json.Get<int>("rate_limit_per_user");
        category_id = json.GetIDSafely("parent_id");

        if (json.ContainsNotNull("recipients")) {
            json["recipients"].IterateThrough([&] (const discpp::JsonObject& user_json)->bool {
                recipients.emplace_back(user_json);
                return true;
            });
        }

        if (json.ContainsNotNull("icon")) {
            std::string icon_str = json["icon"].GetString();

            if (StartsWith(icon_str, "a_")) {
                is_icon_gif = true;
                SplitAvatarHash(icon_str.substr(2), icon_hex);
            } else {
                SplitAvatarHash(icon_str, icon_hex);
            }
        }

        owner_id = json.GetIDSafely("owner_id");
        application_id = json.GetIDSafely("application_id");
	}

	discpp::Message Channel::Send(const std::string& text, const bool tts, discpp::EmbedBuilder* embed, std::vector<File> files) {
        // Send a file filled with message contents if the message is more than 2000 characters.
        if (text.size() >= 2000) {
            // Write message to file
            std::ofstream message("message.txt", std::ios::out | std::ios::binary);
            message << text;
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
            } catch (const std::runtime_error& e) {
                // Delete the temporary message file and then throw this exception again.
                remove("message.txt");

                throw std::runtime_error(e);
            }

            return sent_message;
        }

#ifdef RAPIDJSON_BACKEND
        rapidjson::Document message_json(rapidjson::kObjectType);
        message_json.AddMember("content", text, message_json.GetAllocator());
        message_json.AddMember("tts", tts, message_json.GetAllocator());
#elif SIMDJSON_BACKEND

#endif

        if (embed != nullptr) {
#ifdef RAPIDJSON_BACKEND
            rapidjson::Value embed_value(rapidjson::kObjectType);
            embed_value.CopyFrom(embed->embed_json, message_json.GetAllocator());

            message_json.AddMember("embed", embed_value, message_json.GetAllocator());
#elif SIMDJSON_BACKEND

#endif
        }

        if (!files.empty()) {
            cpr::Multipart multipart_data{};

            for (int i = 0; i < files.size(); i++) {
                multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
            }

#ifdef RAPIDJSON_BACKEND
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            message_json.Accept(writer);

            globals::client_instance->logger->Debug("Sending payload_json inside multipart data for files: " + std::string(buffer.GetString()));

            multipart_data.parts.emplace_back("payload_json", buffer.GetString());
#elif SIMDJSON_BACKEND

#endif

            WaitForRateLimits(id, RateLimitBucketType::CHANNEL);

            cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/channels/" + std::to_string(id) + "/messages") }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
            globals::client_instance->logger->Debug("Received requested payload: " + response.text);

            HandleRateLimits(response.header, id, RateLimitBucketType::CHANNEL);

            return discpp::Message(discpp::JsonObject(response.text));
        }

#ifdef RAPIDJSON_BACKEND
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        message_json.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
        std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/messages"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        return discpp::Message(*result);
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

#ifdef RAPIDJSON_BACKEND
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        j_body.Accept(writer);
        cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif
		std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/channels/" + std::to_string(id)), headers, id, RateLimitBucketType::CHANNEL, body);
		
		*this = discpp::Channel(*result);
		return *this;
	}

	void Channel::Delete() {
		std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	std::vector<discpp::Message> Channel::RequestMessages(int amount, RequestChannelsMessageMethod get_method) const {
	    std::string url = Endpoint("/channels/" + std::to_string(id) + "/messages?limit=" + std::to_string(amount));

	    if (get_method.around_id != 0) {
            url += "&around=" + std::to_string(get_method.around_id);
	    } else if (get_method.before_id != 0) {
            url += "&before=" + std::to_string(get_method.before_id);
        } else if (get_method.after_id != 0) {
            url += "&after=" + std::to_string(get_method.after_id);
        }

		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(url, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

		std::vector<discpp::Message> messages;
		result->IterateThrough([&] (const discpp::JsonObject& message_json)->bool {
            messages.emplace_back(message_json);
            return true;
		});

		return messages;
	}

	void Channel::TriggerTypingIndicator() {
		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/typing"), DefaultHeaders(), {}, {});
	}

	std::optional<std::vector<discpp::Message>> Channel::GetPinnedMessages() {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) = "/pins"), DefaultHeaders(), {}, {});

        std::optional<std::vector<discpp::Message>> messages;
        try {
            for (auto &message : result->GetArray()) {
                rapidjson::Document message_json;
                message_json.CopyFrom(message, message_json.GetAllocator());
                messages->push_back(discpp::Message(message_json));
            }
        } catch (std::exception& e) {
            messages = std::nullopt;
        }

        return messages;
    }

    discpp::Channel Channel::RequestChannel(discpp::Snowflake id) {
        std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
        return discpp::Channel(*result);
    }

	void Channel::BulkDeleteMessage(const std::vector<Snowflake>& messages) {
        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            throw std::runtime_error("discpp::Channel::BulkDeleteMessage only available for guild channels!");
        }

		std::string endpoint = Endpoint("/channels/" + std::to_string(id) + "/messages/bulk-delete");

		std::string combined_message = "";
		for (Snowflake message : messages) {
			if (message == messages[0]) {
				combined_message += "\"" + std::to_string(message) + "\"";
			} else {
				combined_message += ", \"" + std::to_string(message) + "\"";
			}
		}

		cpr::Body body("{\"messages\": [" + combined_message + "]}");
		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
	}

    void Channel::DeletePermission(const discpp::Permissions& permissions) {
        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            throw std::runtime_error("discpp::Channel::DeletePermission only available for guild channels!");
        }

        SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/permissions/" + std::to_string(permissions.role_user_id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
    }

    void Channel::EditPermissions(const discpp::Permissions& permissions) {
        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            throw std::runtime_error("discpp::Channel::EditPermissions only available for guild channels!");
        }

        std::string s_type = (permissions.permission_type == PermissionType::MEMBER) ? "member" : "role";

#ifdef RAPIDJSON_BACKEND
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
#elif SIMDJSON_BACKEND

#endif
        SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/permissions/" + std::to_string(permissions.role_user_id)), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, cpr::Body(json_payload));
    }

    std::optional<std::shared_ptr<discpp::Guild>> Channel::GetGuild() const {
	    std::optional<std::shared_ptr<discpp::Guild>> tmp;

        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            tmp = std::nullopt;
            //throw exceptions::ProhibitedEndpointException("discpp::Channel::GetGuild only available for guild channels!");
        } else {
            tmp = globals::client_instance->cache.GetGuild(guild_id);
        }

        return tmp;
    }

    discpp::GuildInvite Channel::CreateInvite(const int& max_age, const int& max_uses, const bool temporary, const bool unique) {
        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            throw std::runtime_error("discpp::Channel::CreateInvite only available for guild channels!");
        }

        cpr::Body body("{\"max_age\": " + std::to_string(max_age) + ", \"max_uses\": " + std::to_string(max_uses) + ", \"temporary\": " + std::to_string(temporary) + ", \"unique\": " + std::to_string(unique) + "}");
        std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders({ {"Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
        discpp::GuildInvite invite(*result);

        return invite;
    }

	std::optional<std::vector<discpp::GuildInvite>> Channel::GetInvites() {
	    std::optional<std::vector<discpp::GuildInvite>> tmp;
        if (type == ChannelType::GROUP_DM || type == ChannelType::DM) {
            tmp = std::nullopt;
            //throw std::runtime_error("discpp::Channel::GetInvites only available for guild channels!");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(id) + "/invites"), DefaultHeaders(), {}, {});
            for (auto& invite : result->GetArray()) {
                rapidjson::Document invite_json;
                invite_json.CopyFrom(invite, invite_json.GetAllocator());
                tmp->push_back(discpp::GuildInvite(invite_json));
            }
        }

		return tmp;
	}

	std::optional<std::unordered_map<discpp::Snowflake, discpp::Channel>> Channel::GetChildren() {
	    std::optional<std::unordered_map<discpp::Snowflake, discpp::Channel>> tmp;
        if (type != ChannelType::GROUP_CATEGORY) {
            tmp = std::nullopt;
            /*
            globals::client_instance->logger->Debug(LogTextColor::RED + "discpp::Channel::GetChildren only available for category channels!");
            throw std::runtime_error("discpp::Channel::GetChildren only available for category channels!");
             */
        } else {
            if (this->GetGuild().has_value()) {
                for (auto const chnl : this->GetGuild()->get()->channels) {
                    if (chnl.second.category_id == this->id) {
                        tmp->insert({ chnl.first, chnl.second });
                    } else {
                        continue;
                    }
                }
            } else {
                tmp = std::nullopt;
            }
        }

	    return tmp;
	}

	void Channel::GroupDMAddRecipient(const discpp::User& user) {
	    if (type == ChannelType::DM || type == ChannelType::GROUP_DM) {
		    SendPutRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		} else {
            globals::client_instance->logger->Debug(LogTextColor::RED + "discpp::Channel::GroupDMAddRecipient only available for DM/Group DM channels!");
	        throw std::runtime_error("discpp::Channel::GroupDMAddRecipient only available for DM/Group DM channels!");
	    }
	}

	void Channel::GroupDMRemoveRecipient(const discpp::User& user) {
        if (type == ChannelType::DM || type == ChannelType::GROUP_DM) {
            SendDeleteRequest(Endpoint("/channels/" + std::to_string(id) + "/recipients/" + std::to_string(user.id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
        } else {
            globals::client_instance->logger->Debug(LogTextColor::RED + "discpp::Channel::GroupDMRemoveRecipient only available for DM/Group DM channels!");
            throw std::runtime_error("discpp::Channel::GroupDMRemoveRecipient only available for DM/Group DM channels!");
        }
	}

    std::optional<discpp::Message> Channel::RequestMessage(discpp::Snowflake id) {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(this->id) + "/messages/" + std::to_string(id)), DefaultHeaders(), {}, {});

        std::optional<discpp::Message> tmp;
        try {
            tmp = discpp::Message(*result);
        } catch (std::exception& e) {
            tmp = std::nullopt;
        }

        return tmp;
    }

    std::string Channel::GetIconURL(const ImageType &img_type) const {
        std::string icon_str = CombineAvatarHash(icon_hex);

        std::string url = "https://cdn.discordapp.com/channel-icons/" + std::to_string(id) + "/" + icon_str;
        ImageType tmp = img_type;
        if (tmp == ImageType::AUTO) tmp = is_icon_gif ? ImageType::GIF : ImageType::PNG;
        switch (img_type) {
            case ImageType::GIF:
                return cpr::Url(url + ".gif");
            case ImageType::JPEG:
                return cpr::Url(url + ".jpeg");
            case ImageType::PNG:
                return cpr::Url(url + ".png");
            case ImageType::WEBP:
                return cpr::Url(url + ".webp");
            default:
                return cpr::Url(url);
        }
    }
}