#include "context.h"

discord::Context::Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::vector<std::string> arguments) : bot(bot), guild(message.guild), channel(channel), author(author), user(author.user), message(message), arguments(arguments) {
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

discord::Message discord::Context::Send(std::string text, bool tts) {
	/**
	 * @brief Sends a text message (Shortcut for `channel.send(text, tts)`).
	 *
	 * ```cpp
	 *      ctx.Send("Hello, I'm a bot!");
	 * ```
	 *
	 * @param[in] text The text that will be sent.
	 * @param[in] tts If the text could be spoken through text to speach.
	 *
	 * @return discord::Message
	 */

	return channel.Send(text, tts);
}

discord::Message discord::Context::Send(discord::EmbedBuilder embed, std::string text) {
	/**
	 * @brief Sends an embed (Shortcut for `channel.send(embed, text)`).
	 *
	 * ```cpp
	 *      ctx.Send(embed, "Look at the embed I sent!");
	 * ```
	 *
	 * @param[in] embed The embed to send.
	 * @param[in] text The text to send.
	 *
	 * @return discord::Message
	 */

	return channel.Send(embed, text);
}

discord::Message discord::Context::Send(std::vector<discord::File> files, std::string text) {
	/**
	 * @brief Sends files (Shortcut for `channel.send({file_a, file_b}, text)`).
	 *
	 * ```cpp
	 *      ctx.Send({file_a, file_b}, "Read these files real quick");
	 * ```
	 *
	 * @param[in] files The files that will be sent.
	 * @param[in] text The text that will be sent.
	 *
	 * @return discord::Message
	 */

	return channel.Send(files, text);
}
