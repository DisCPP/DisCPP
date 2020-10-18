#include "command.h"
#include "command_handler.h"
#include "client_config.h"

void discpp::FireCommand(discpp::Shard& shard, const discpp::Message& message) {
    if (message.author.IsBot()) return;

    size_t prefix_size = 0;
    bool trigger = false;

    for (std::string const& prefix : shard.client.config.prefixes) {
        if (prefix.empty()) continue;

        if (StartsWith(message.content, prefix)) {
            prefix_size = prefix.size();
            trigger = true;
            break;
        }
    }
    if (!trigger) {
        return;
    }

    // Remove the prefix from the message
    std::string content = message.content.substr(prefix_size);
    std::vector<std::string> argument_vec = discpp::SplitString(content, " ");

    auto found_command = shard.client.command_handler->registered_commands.find(argument_vec[0]);
    if (found_command == shard.client.command_handler->registered_commands.end()) return;

    argument_vec.erase(argument_vec.begin()); // Erase the command from the arguments

    std::shared_ptr<discpp::Member> member = message.member;

    std::string remainder;
    if (!argument_vec.empty()) remainder = argument_vec.size() == 1 ? argument_vec[0] : discpp::CombineStringVector(argument_vec, " ", 0);

    Context context = Context(shard, message.channel, member, message, remainder, argument_vec);

    if (!found_command->second->registered_commands.empty()) {
        found_command->second->SubCommandHandler(context);
    } else {
        if (!found_command->second->CanRun(context)) return;
        found_command->second->CommandBody(context);
    }
}