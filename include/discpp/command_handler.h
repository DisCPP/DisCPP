#ifndef DISCPP_COMMAND_HANDLER_H
#define DISCPP_COMMAND_HANDLER_H

#include "client.h"
#include "context.h"
#include "message.h"
#include "command.h"

#include <memory>

namespace discpp {
	inline std::unordered_map<std::string, Command*> registered_commands;

    /**
     * @brief Detects if a command has ran, and if it has then execute it.
     *
     * @param[in] bot A reference to the discpp bot.
     * @param[in] message The message that was sent.
     *
     * @return void
     */
	void FireCommand(discpp::Client* bot, const discpp::Message& message);
}

#endif