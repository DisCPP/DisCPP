#ifndef DISCORDPP_CONTEXT_H
#define DISCORDPP_CONTEXT_H

#include "bot.h"
#include "channel.h"
#include "message.h"

namespace discord {
	class Context {
	private:

	public:
		discord::Bot* bot;
		discord::Channel channel;
		discord::User user;
		discord::Member author;
		discord::Message message;
		std::vector<std::string> arguments;

		Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::vector<std::string> arguments);

		discord::Message Send(std::string text, bool tts = false) {
			return channel.Send(text, tts);
		}

		discord::Message Send(discord::EmbedBuilder embed, std::string text = "") {
			return channel.Send(embed, text);
		}
	};
}

#endif