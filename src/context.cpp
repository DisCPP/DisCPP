#include "context.h"

discord::Context::Context(discord::Bot* bot, discord::Channel channel, discord::Member author, discord::Message message, std::vector<std::string> arguments) : bot(bot), channel(channel), author(author), user(author.user), message(message), arguments(arguments) {

}