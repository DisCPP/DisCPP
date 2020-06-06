#ifndef DISCPP_WEBHOOK_H
#define DISCPP_WEBHOOK_H

#include <string>
#include <discpp/channel.h>
#include <discpp/embed_builder.h>

namespace discpp {
	class Guild;
	class Channel;
	class User;

    enum WebhookType : int {
        INCOMING = 1,
        CHANNEL_FOLLOWER = 2
    };

    // @TODO: Add more endpoints: https://discordapp.com/developers/docs/resources/webhook
	class Webhook : public DiscordObject {
	public:
	    Webhook() = default;
	    Webhook(rapidjson::Document& json);
		Webhook(const Snowflake& id, const std::string& token);

		discpp::Message Send(const std::string& text, const bool& tts = false, discpp::EmbedBuilder* embed = nullptr, const std::vector<discpp::File>& files = {});
		void EditName(std::string& name);
		void Remove();

        WebhookType type;
        std::shared_ptr<discpp::Guild> guild;
        std::shared_ptr<discpp::Channel> channel;
        std::shared_ptr<discpp::User> user;
        std::string name;
        std::string avatar;
        std::string token;
	};
}

#endif