#include "webhook.h"
#include "utils.h"
#include "message.h"
#include "client.h"
#include "guild.h"

#include <fstream>

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

		std::string escaped_text = EscapeString(text);
        rapidjson::Document message_json = rapidjson::Document(rapidjson::kObjectType);
        message_json.Parse("{\"content\":\"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");

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
		    std::unique_ptr<rapidjson::Document> json = embed->ToJson();
			body = cpr::Body("{\"embed\": " + DumpJson(*json) + ((!text.empty()) ? ", \"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") : "") + "}");
		}
		else if (!files.empty()) {
			cpr::Multipart multipart_data{};

			for (int i = 0; i < files.size(); i++) {
				multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
			}

			multipart_data.parts.emplace_back("payload_json", "{\"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "\"}");

			WaitForRateLimits(nullptr, id, RateLimitBucketType::WEBHOOK);
			cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/webhooks/" + std::to_string(id) + "/" + token) }, Headers({ {"Content-Type", "multipart/form-data"} }), multipart_data);
            HandleRateLimits(response.header, id, RateLimitBucketType::WEBHOOK);

            rapidjson::Document result_json(rapidjson::kObjectType);
            result_json.Parse(response.text);
			return discpp::Message(nullptr, result_json);
		} else {
			body = cpr::Body(DumpJson(message_json));
		}
		std::unique_ptr<rapidjson::Document> result = SendPostRequest(nullptr, Endpoint("/webhooks/" + std::to_string(id) + "/" + token), Headers({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::WEBHOOK, body);

		return discpp::Message(nullptr, *result); // @TODO: Make WebhookMessage
	}

	void Webhook::EditName(std::string& name) {
        rapidjson::Document result_json(rapidjson::kObjectType);
        result_json.Parse("{\"name\": \"" + name + "\"}");
		discpp::SendPatchRequest(nullptr, discpp::Endpoint("/webhooks/" + std::to_string(id)), Headers({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK, cpr::Body(DumpJson(result_json)));
	}

	void Webhook::Remove() {
		discpp::SendDeleteRequest(nullptr, discpp::Endpoint("/webhooks/" + std::to_string(id) + "/" + token), Headers({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK);
	}

    cpr::Header Webhook::Headers(const cpr::Header& add) {
        cpr::Header headers = { { "User-Agent", "DisC++ Webhook (https://github.com/seanomik/DisCPP, v0.0.0)" },
                                { "X-RateLimit-Precision", "millisecond"} };
        return headers;
    }
}