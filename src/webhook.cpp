#include "webhook.h"
#include "utils.h"
#include "message.h"
#include "client.h"
#include "guild.h"
#include "log.h"

#include <fstream>

#ifdef RAPIDJSON_BACKEND
#include <rapidjson/writer.h>
#elif SIMDJSON_BACKEND

#endif

namespace discpp {
    Webhook::Webhook(const discpp::JsonObject& json) {
        id = SnowflakeFromString(json["id"].GetString());
        type = static_cast<WebhookType>(json["type"].GetInt());
        guild = std::make_shared<discpp::Guild>(json.ConstructDiscppObjectFromID("guild_id", discpp::Guild()));
        channel = std::make_shared<discpp::Channel>(globals::client_instance->cache.GetChannel(SnowflakeFromString(json["guild_id"].GetString())));
		user = std::make_shared<discpp::User>(json.ConstructDiscppObjectFromJson("user", discpp::User()));
        name = json.Get<std::string>("name");
        if (json.ContainsNotNull("avatar")) {
            SplitAvatarHash(json["avatar"].GetString(), avatar_hex);
        }
        token = json.Get<std::string>("token");
    }

	Webhook::Webhook(const discpp::Snowflake& id, const std::string& token) : DiscordObject(id) {
		this->token = token;
		this->id = id;
	};

	discpp::Message Webhook::Send(const std::string& text, const bool tts, discpp::EmbedBuilder* embed, const std::vector<discpp::File>& files) {
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

            cpr::Response response = cpr::Post(Endpoint("/webhooks/" + std::to_string(id) + "/" + token), DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
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
        std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/webhooks/" + std::to_string(id) + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        return discpp::Message(*result);
	}

	void Webhook::EditName(std::string& name) {
        cpr::Body body("{\"name\": \"" + name + "\"}");
		discpp::SendPatchRequest(discpp::Endpoint("/webhooks/" + std::to_string(id)), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK, body);
	}

	void Webhook::Remove() {
		discpp::SendDeleteRequest(discpp::Endpoint("/webhooks/" + std::to_string(id) + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK);
	}
}