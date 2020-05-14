#ifndef DISCPP_CONTEXT_H
#define DISCPP_CONTEXT_H

#include <vector>
#include <string>

#include "client.h"
#include "guild.h"
#include "channel.h"
#include "user.h"
#include "member.h"
#include "message.h"

namespace discpp {

	class Context {
	private:

	public:
		discpp::Client* client;
		std::shared_ptr<discpp::Guild> guild;
		discpp::Channel channel;
        std::shared_ptr<discpp::User> user;
        std::shared_ptr<discpp::Member> author;
		discpp::Message message;
		std::string remainder;
		std::vector<std::string> arguments;

		Context(discpp::Client* client, discpp::Channel channel, std::shared_ptr<discpp::Member> author, discpp::Message message, std::string remainder, std::vector<std::string> arguments);

		discpp::Message Send(std::string text, bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});
	};
}

#endif