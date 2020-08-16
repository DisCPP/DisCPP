#ifndef DISCPP_WEBHOOK_H
#define DISCPP_WEBHOOK_H

#include <string>

#include "channel.h"
#include "embed_builder.h"
#include "guild.h"
#include "user.h"

namespace discpp {

    enum WebhookType : int {
        INCOMING = 1,
        CHANNEL_FOLLOWER = 2
    };

    // @TODO: Add more endpoints: https://discordapp.com/developers/docs/resources/webhook
	class Webhook {
	public:
	    Webhook() = default;
	    Webhook(rapidjson::Document& json);
		Webhook(const Snowflake& id, const std::string& token);

		discpp::Message Send(const std::string& text, const bool tts = false, discpp::EmbedBuilder* embed = nullptr, const std::vector<discpp::File>& files = {});
		void EditName(std::string& name);
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

        cpr::Header Headers(const cpr::Header& add = {});
	};
}

#endif