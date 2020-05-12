#include "context.h"

discpp::Context::Context(discpp::Client* client, std::shared_ptr<discpp::Channel> channel, std::shared_ptr<discpp::Member> author, discpp::Message message, std::string remainder, std::vector<std::string> arguments) : client(client), guild(message.guild), channel(channel), author(author), user(message.author), message(message), remainder(remainder), arguments(arguments) {
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
}

discpp::Message discpp::Context::Send(std::string text, bool tts, discpp::EmbedBuilder* embed, std::vector<discpp::File> files) {
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
	 * @param[in] tts If the text could be spoken through text to speach.
	 * @param[in] embed Optional embed to send
	 * @param[in] files Option files to send
	 *
	 * @return discpp::Message
	 */

	return channel->Send(text, tts, embed, files);
}
