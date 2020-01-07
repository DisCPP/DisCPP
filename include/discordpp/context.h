#ifndef DISCORDPP_CONTEXT_H
#define DISCORDPP_CONTEXT_H

#include <vector>
#include <string>

#include "bot.h"
#include "guild.h"
#include "channel.h"
#include "user.h"
#include "member.h"
#include "message.h"

namespace discord {

	class Context {
	private:

	public:
		discord::Bot* bot;
		discord::Guild guild;
		discord::Channel channel;
		discord::User user;
		discord::Member author;
		discord::Message message;
		std::vector<std::string> arguments;

		Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::vector<std::string> arguments);

		discord::Message Send(std::string text, bool tts = false);
		discord::Message Send(discord::EmbedBuilder embed, std::string text = "");
		discord::Message Send(std::vector<discord::File> files, std::string text = "");
	};
}

#endif