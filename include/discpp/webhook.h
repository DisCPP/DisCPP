#ifndef DISCPP_WEBHOOK_H
#define DISCPP_WEBHOOK_H

#include <string>
#include <discpp\channel.h>
#include <discpp\embed_builder.h>

namespace discpp {
	class WebhookClient : DiscordObject {
	public:
		WebhookClient(snowflake id, std::string token);
		std::string token;

		discpp::Message Send(std::string text, bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});
		void EditName(std::string& name);
		void Remove();
	};
}

#endif