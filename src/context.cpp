#include "context.h"

discord::Context::Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::string remainder, std::vector<std::string> arguments) : bot(bot), guild(message.guild), channel(channel), author(author), user(author.user), message(message), remainder(remainder), arguments(arguments) {
	/**
	 * @brief Constructs a discord::Context object.
	 *
	 * ```cpp
	 *      discord::Context ctx(bot, channel, author, message, arguments);
	 * ```
	 *
	 * @param[in] bot The discord bot.
	 * @param[in] channel The channel the command was ran in.
	 * @param[in] author The author of the message that executed the command.
	 * @param[in] message The message that was sent by the player
	 * @param[in] arguments The command arguments
	 *
	 * @return discord::Context, this is a constructor.
	 */
}

discord::Message discord::Context::Send(std::string text, bool tts, discord::EmbedBuilder* embed, std::vector<discord::File> files) {
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
	 * @return discord::Message
	 */

	return channel.Send(text, tts, embed, files);
}
