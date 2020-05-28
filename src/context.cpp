#include "context.h"
#include <utility>

discpp::Context::Context(discpp::Client* client, discpp::Channel channel, std::shared_ptr<discpp::Member> author, const discpp::Message& message, std::string remainder, std::vector<std::string> arguments) : client(client), guild(message.guild), channel(std::move(channel)), author(std::move(author)), user(message.author), message(message), remainder(std::move(remainder)), arguments(std::move(arguments)) {}

discpp::Message discpp::Context::Send(const std::string& text, const bool& tts, discpp::EmbedBuilder* embed, std::vector<discpp::File> files) {
	return channel.Send(text, tts, embed, files);
}
