#include "context.h"

discord::Context::Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::vector<std::string> arguments) : bot(bot), guild(message.guild), channel(channel), author(author), user(author.user), message(message), arguments(arguments) {

}

discord::Message discord::Context::Send(std::string text, bool tts) {
	return channel.Send(text, tts);
}

discord::Message discord::Context::Send(discord::EmbedBuilder embed, std::string text) {
	return channel.Send(embed, text);
}

discord::Message discord::Context::Send(std::vector<discord::File> files, std::string text) {
	return channel.Send(files, text);
}
