#ifndef DISCORDPP_COMMAND_HANDLER_H
#define DISCORDPP_COMMAND_HANDLER_H

#include "bot.h"
#include "context.h"
#include "message.h"
#include "command.h"

#include <memory>

namespace discord {
	inline std::unordered_map<std::string, Command*> registered_commands;

	void FireCommand(discord::Bot* bot, discord::Message message);
}

#endif