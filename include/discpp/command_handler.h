#ifndef DISCPP_COMMAND_HANDLER_H
#define DISCPP_COMMAND_HANDLER_H

#include "client.h"
#include "context.h"
#include "message.h"
#include "command.h"

#include <memory>
#include <type_traits>

namespace discpp {
    class CommandHandler : public std::enable_shared_from_this<CommandHandler> {
        Client &client;
        friend class Client;
        friend class Command;
      public:
        std::unordered_map<std::string, std::shared_ptr<Command>> registered_commands;

        CommandHandler(Client &parent) : client(parent) {}

        template <typename T, typename... Args, std::enable_if_t<std::is_base_of<Command, T>::value || std::is_same<T, Command>::value> * = nullptr>
        void RegisterCommand(Args &&... args)
        {
            auto command = std::make_shared<T>(args...);

            command->parent = shared_from_this();

            registered_commands.insert({command->name, command});
            for (auto &alias : command->aliases)
            {
                registered_commands.insert({alias, command});
            }
        };
    };
    /**
     * @brief Detects if a command has ran, and if it has then execute it.
     *
     * @param[in] bot A reference to the discpp bot.
     * @param[in] message The message that was sent.
     *
     * @return void
     */
	void FireCommand(discpp::Shard& shard, const discpp::Message& message);
}

#endif