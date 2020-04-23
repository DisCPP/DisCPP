#ifndef DISCPP_COMMAND_HANDLER_H
#define DISCPP_COMMAND_HANDLER_H

#include "bot.h"
#include "context.h"
#include "message.h"
#include "command.h"

#include <memory>

namespace discpp {
	inline std::unordered_map<std::string, Command*> registered_commands;

	void FireCommand(discpp::Bot* bot, discpp::Message message);
}

#endif