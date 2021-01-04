#ifndef DISCPP_WEBHOOK_H
#define DISCPP_WEBHOOK_H

#include <string>

#include "channel.h"
#include "embed_builder.h"
#include "guild.h"
#include "user.h"
#include "http_client.h"

namespace discpp {
    class HttpClient;

    enum WebhookType : int {
        INCOMING = 1,
        CHANNEL_FOLLOWER = 2
    };

    // @TODO: Add more endpoints: https://discordapp.com/developers/docs/resources/webhook
	class Webhook {
	public:
	    Webhook(std::shared_ptr<HttpClient> http_client);
	    Webhook(std::shared_ptr<HttpClient> http_client, rapidjson::Document& json);
		Webhook(std::shared_ptr<HttpClient> http_client, const Snowflake& id, const std::string& token);

		discpp::Message Send(const std::string& text, const bool tts = false, discpp::EmbedBuilder* embed = nullptr, const std::vector<discpp::File>& files = {});
		void EditName(const std::string &name);
		void Remove();

        discpp::Snowflake id = 0;
        WebhookType type;
        discpp::Snowflake guild_id;
        discpp::Snowflake channel_id;
        discpp::User user;
        std::string name;
        std::string token;
	private:
        uint64_t avatar_hex[2] = {0, 0};
        std::shared_ptr<HttpClient> http_client;

        std::map<std::string, std::string, discpp::CaseInsensitiveLess> Headers(const std::map<std::string, std::string, discpp::CaseInsensitiveLess>& add = {});
	};
}

#endif