#include "webhook.h"
#include "utils.h"
#include "message.h"

#include <fstream>

namespace discpp {
	WebhookClient::WebhookClient(discpp::snowflake id, std::string token) : DiscordObject(id) {
		this->token = token;
		this->id = id;
	};

	discpp::Message WebhookClient::Send(std::string text, bool tts, discpp::EmbedBuilder* embed, std::vector<discpp::File> files) {

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
		nlohmann::json result = SendPostRequest(Endpoint("/webhooks/" + id + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::WEBHOOK, body);

		return discpp::Message(result);
	}

	void WebhookClient::EditName(std::string& name) {
		nlohmann::json j = { "name", name };
		discpp::SendPatchRequest(discpp::Endpoint("/webhooks/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK, cpr::Body(j.dump()));
	}

	void WebhookClient::Remove() {
		discpp::SendDeleteRequest(discpp::Endpoint("/webhooks/" + id + "/" + token), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discpp::RateLimitBucketType::WEBHOOK);
	}
}