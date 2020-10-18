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
		discpp::Shard& shard;
		std::shared_ptr<discpp::Guild> guild;
        discpp::Channel channel;
        discpp::User user;
        std::shared_ptr<discpp::Member> author;
		discpp::Message message;
		std::string remainder;
		std::vector<std::string> arguments;

        /**
         * @brief Constructs a discpp::Context object.
         *
         * ```cpp
         *      discpp::Context ctx(bot, channel, author, message, arguments);
         * ```
         *
         * @param[in] bot The discpp bot.
         * @param[in] channel The channel the command was ran in.
         * @param[in] author The author of the message that executed the command.
         * @param[in] message The message that was sent by the player
         * @param[in] arguments The command arguments
         *
         * @return discpp::Context, this is a constructor.
         */
		Context(discpp::Shard& shard, discpp::Channel channel, std::shared_ptr<discpp::Member> author, const discpp::Message& message, std::string remainder, std::vector<std::string> arguments);

        /**
         * @brief Sends a text message (Shortcut for `channel.send(text, tts, embed, files)`).
         *
         * ```cpp
         *      ctx.Send("Hello, I'm a bot!"); // Sending text
         *      ctx.Send("", false, embed); // Sending an embed
         *      ctx.Send("Command output was too large to fit in an embed.", false, nullptr, { file }); // Sending files
         * ```
         *
         * @param[in] text The text that will be sent.
         * @param[in] tts If the text could be spoken through text to speech.
         * @param[in] embed Optional embed to send
         * @param[in] files Option files to send
         *
         * @return discpp::Message
         */
		discpp::Message Send(const std::string& text, const bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});
	};
}

#endif