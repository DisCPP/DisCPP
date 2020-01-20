#ifndef DISCORDPP_COMMAND_H
#define DISCORDPP_COMMAND_H

#include "context.h"

#include <string>
#include <vector>
#include <functional>

namespace discord {
	class Command {
	public:

		Command() = default;
		Command(std::string name);
		Command(std::string name, std::string desc, std::vector<std::string> hint_args, std::function<void(discord::Context)> function, std::vector<std::function<bool(discord::Context)>> requirements);

		virtual void CommandBody(discord::Context ctx);
		virtual bool CanRun(discord::Context ctx);

		std::function<void(discord::Context)> function = nullptr;
		std::string name;
		std::string description;
		std::vector<std::string> hint_args;
		std::vector<std::function<bool(discord::Context)>> requirements;
	};
}

#endif