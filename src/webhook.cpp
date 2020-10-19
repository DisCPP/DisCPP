#include "webhook.h"
#include "utils.h"
#include "message.h"
#include "client.h"

#include <fstream>
#include <sstream>

#include <ixwebsocket/IXHttpClient.h>

/* @TODO: Create a WebhookMessage, WebhookChannel, and add a logger.
 * WebhookChannel will override the discpp::Channel::Send method to modify it for webhooks.
 */

namespace discpp {
    Webhook::Webhook(rapidjson::Document& json) {
        id = Snowflake(json["id"].GetString());
        type = static_cast<WebhookType>(json["type"].GetInt());
        guild_id = Snowflake(json["guild_id"].GetString());
        channel_id = Snowflake(json["channel_id"].GetString());
		user = ConstructDiscppObjectFromJson(nullptr, json, "user", discpp::User());
        name = GetDataSafely<std::string>(json, "name");
        if (ContainsNotNull(json, "avatar")) {
            SplitAvatarHash(json["avatar"].GetString(), avatar_hex);
        }
        token = GetDataSafely<std::string>(json, "token");
    }

	Webhook::Webhook(const discpp::Snowflake& id, const std::string& token) : id(id) {
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
            discpp::Message sent_message(nullptr);
            try {
                // Send the message
                std::vector<discpp::File> files;
                files.push_back({ "message.txt", "message.txt" });
                sent_message = Send("Message was too large to fit in 2000 characters", tts, embed, files);

                // Delete the temporary message file
                remove("message.txt");
            } catch (const std::runtime_error& e) {
                // Delete the temporary message file and then throw this exception again.
                remove("message.txt");

                throw std::runtime_error(e);
            }

            return sent_message;
        }

        rapidjson::Document message_json(rapidjson::kObjectType);
        message_json.AddMember("content", text, message_json.GetAllocator());
        message_json.AddMember("tts", tts, message_json.GetAllocator());

        if (embed != nullptr) {
            rapidjson::Value embed_value(rapidjson::kObjectType);
            embed_value.CopyFrom(embed->embed_json, message_json.GetAllocator());

            message_json.AddMember("embed", embed_value, message_json.GetAllocator());
        }

        if (!files.empty()) {
            auto* http_client = new ix::HttpClient();

            ix::HttpRequestArgsPtr args = http_client->createRequest();

            ix::HttpFormDataParameters data_parameters;
            for (int i = 0; i < files.size(); i++) {
                const discpp::File& file = files[i];
                std::ifstream file_stream(file.file_path, std::ios::in | std::ios::binary);
                std::ostringstream ss;
                ss << file_stream.rdbuf();

                if (file.file_name.empty()) {
                    data_parameters["file_" + std::to_string(i)] = ss.str();
                } else {
                    data_parameters[file.file_name] = ss.str();
                }
            }

            data_parameters["payload_json"] = discpp::DumpJson(message_json);

            args->verbose = false;
            args->compress = false;
            args->extraHeaders = Headers();

            // Generate a body from the multipart using IXWebsocket's method but then modify it
            // so the payload_json field will actually be send as json, and not a file.
            std::string multipart_bound = http_client->generateMultipartBoundary();
            args->multipartBoundary = multipart_bound;
            std::string body = http_client->serializeHttpFormDataParameters(multipart_bound, data_parameters);
            discpp::ReplaceAll(body, "Content-Disposition: form-data; name=\"payload_json\"; filename=\"payload_json\"\r\nContent-Type: application/octet-stream", "Content-Disposition: form-data; name=\"payload_json\"\r\nContent-Type: application/json");

            WaitForRateLimits(nullptr, id, RateLimitBucketType::CHANNEL);

            ix::HttpResponsePtr result = http_client->post(Endpoint("/webhooks/" + std::to_string(id) + "/" + token), body, args);


            rapidjson::Document result_json(rapidjson::kObjectType);
            result_json.Parse(result->body);

            return discpp::Message(nullptr, result_json);
        }

        std::unique_ptr<rapidjson::Document> result = SendPostRequest(nullptr, Endpoint("/webhooks/" + std::to_string(id) + "/" + token), DefaultHeaders(nullptr, { { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, DumpJson(message_json));

		return discpp::Message(nullptr, *result); // @TODO: Make WebhookMessage
	}

	void Webhook::EditName(std::string& name) {
        rapidjson::Document result_json(rapidjson::kObjectType);
        result_json.Parse("{\"name\": \"" + name + "\"}");
		discpp::SendPatchRequest(nullptr, discpp::Endpoint("/webhooks/" + std::to_string(id)), Headers({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK, DumpJson(result_json));
	}

	void Webhook::Remove() {
		discpp::SendDeleteRequest(nullptr, discpp::Endpoint("/webhooks/" + std::to_string(id) + "/" + token), Headers({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK);
	}

    ix::WebSocketHttpHeaders Webhook::Headers(const ix::WebSocketHttpHeaders& add) {
        ix::WebSocketHttpHeaders headers = { { "User-Agent", "DisC++ Webhook (https://github.com/seanomik/DisCPP, v0.0.0)" },
                                { "X-RateLimit-Precision", "millisecond"} };
        return headers;
    }
}