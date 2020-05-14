#include "command.h"
#include "command_handler.h"
#include "client_config.h"

void discpp::FireCommand(discpp::Client* bot, discpp::Message message) {
    /**
     * @brief Detects if a command has ran, and if it has then execute it.
     *
     * @param[in] bot A reference to the discpp bot.
     * @param[in] message The message that was sent.
     *
     * @return void
     */

    int prefixSize = 0;
    bool trigger = false;
    for (std::string const& prefix : bot->config->prefixes) {
        prefixSize = prefix.size();
        if (message.author->IsBot()) {
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

    std::vector<std::string> argument_vec = SplitString(messageContent, " ");
    if (!argument_vec.size()) return;

    auto found_command = registered_commands.find(argument_vec[0]);
    if (found_command == registered_commands.end()) return;

    argument_vec.erase(argument_vec.begin()); // Erase the command from the arguments

    std::shared_ptr<discpp::Member> member = nullptr;
    if (message.channel.type != discpp::ChannelType::DM) {
        member = message.guild->GetMember(message.author->id);
    }

    std::string remainder = "d";
    if (!argument_vec.empty()) remainder = CombineStringVector(argument_vec);

    Context context = Context(bot, message.channel, member, message, remainder, argument_vec);

    if (!found_command->second->CanRun(context)) return;

    found_command->second->CommandBody(context);
}