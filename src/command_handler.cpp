#include "command.h"
#include "command_handler.h"
#include "utils.h"
#include <iostream>

void discord::FireCommand(discord::Bot* bot, discord::Message message) {
    /**
     * @brief Detects if a command has ran, and if it has then execute it.
     *
     * @param[in] bot A reference to the discord bot.
     * @param[in] message The message that was sent.
     *
     * @return void
     */

    int prefixSize = 0;
    bool trigger = false;
    for (std::string const& prefix : bot->prefix) {
        prefixSize = prefix.size();
        if (message.author.bot) {
            return;
        }
        if (StartsWith(message.content, prefix)) {
            trigger = true;
            break;
        }
    }
    if (!trigger) {
        return;
    }

    std::string messageContent = message.content;
    messageContent = messageContent.substr(prefixSize);

    std::vector<std::string> argument_vec = SplitString(messageContent, ' ');
    if (!argument_vec.size()) return;

    auto found_command = registered_commands.find(argument_vec[0]);
    if (found_command == registered_commands.end()) return;

    argument_vec.erase(argument_vec.begin()); // Erase the command from the arguments

    discord::Member member(message.author.id);

    std::string remainder = "d";
    if (!argument_vec.empty()) remainder = CombineVectorWithSpaces(argument_vec, 0);

    Context context = Context(bot, message.channel, member, message, remainder, argument_vec);

    if (!found_command->second->CanRun(context)) return;

    found_command->second->CommandBody(context);
}