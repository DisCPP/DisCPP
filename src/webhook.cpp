#include "webhook.h"
#include "utils.h"
#include "message.h"

#include <fstream>

namespace discpp {
    Webhook::Webhook(rapidjson::Document& json) {
        id = json["id"].GetString();
        type = static_cast<WebhookType>(json["type"].GetInt());
        guild = std::make_shared<discpp::Guild>(ConstructDiscppObjectFromID(json, "guild_id", discpp::Guild()));
		channel = std::make_shared<discpp::Channel>(ConstructDiscppObjectFromID(json, "guild_id", discpp::Channel()));
		user = std::make_shared<discpp::User>(ConstructDiscppObjectFromJson(json, "user", discpp::User()));
        name = GetDataSafely<std::string>(json, "name");
        avatar = GetDataSafely<std::string>(json, "avatar");
        token = GetDataSafely<std::string>(json, "token");
    }

	Webhook::Webhook(discpp::snowflake id, std::string token) : DiscordObject(id) {
		this->token = token;
		this->id = id;
	};

	discpp::Message Webhook::Send(std::string text, bool tts, discpp::EmbedBuilder* embed, std::vector<discpp::File> files) {

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
		    rapidjson::Document json = embed->ToJson();
			body = cpr::Body("{\"embed\": " + DumpJson(json) + ((!text.empty()) ? ", \"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") : "") + "}");
		}
		else if (!files.empty()) {
			cpr::Multipart multipart_data{};

			for (int i = 0; i < files.size(); i++) {
				multipart_data.parts.emplace_back("file" + std::to_string(i), cpr::File(files[i].file_path), "application/octet-stream");
			}

			multipart_data.parts.emplace_back("payload_json", "{\"content\": \"" + escaped_text + (tts ? "\",\"tts\":\"true\"" : "\"") + "\"}");

			WaitForRateLimits(id, RateLimitBucketType::WEBHOOK);
			cpr::Response response = cpr::Post(cpr::Url{ Endpoint("/webhooks/" + id + "/" + token) }, DefaultHeaders({ {"Content-Type", "multipart/form-data"} }), multipart_data);
			HandleRateLimits(response.header, id, RateLimitBucketType::WEBHOOK);

			return discpp::Message(nlohmann::json::parse(response.text));
		}
		else {
			body = cpr::Body(message_json.dump());
		}
		rapidjson::Document result = SendPostRequest(Endpoint("/webhooks/" + id + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::WEBHOOK, body);

		return discpp::Message(result);
	}

	void Webhook::EditName(std::string& name) {
		nlohmann::json j = { "name", name };
		discpp::SendPatchRequest(discpp::Endpoint("/webhooks/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK, cpr::Body(j.dump()));
	}

	void Webhook::Remove() {
		discpp::SendDeleteRequest(discpp::Endpoint("/webhooks/" + id + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK);
	}
}