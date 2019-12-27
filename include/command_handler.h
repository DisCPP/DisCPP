#ifndef DISCORDPP_COMMAND_HANDLER_H
#define DISCORDPP_COMMAND_HANDLER_H

#include "bot.h"
#include "context.h"
#include "message.h"

namespace discord {
	struct Command {
		std::function<void(discord::Context)> function;
		std::string name;
		std::string description;
		std::vector<std::string> hint_args;
		std::vector<std::function<bool(discord::Context)>> requirements;
	};

	namespace { // Private namespace
		std::unordered_map<std::string, Command> registered_commands;
	}

	void FireCommand(discord::Bot* bot, discord::Message message);
	void RegisterCommand(std::string name, std::string description, std::vector<std::string> hint_args, std::function<void(Context)> function, std::vector<std::function<bool(Context)>> requirements);
}

#endif