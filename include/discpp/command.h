#ifndef DISCPP_COMMAND_H
#define DISCPP_COMMAND_H

#include "context.h"

#include <string>
#include <vector>
#include <functional>

namespace discpp {
	class Command {
	public:

		Command() = default;
		Command(const std::string& name);
		Command(const std::string& name, const std::string& desc, const std::vector<std::string>& hint_args, const std::function<void(discpp::Context)>& function, const std::vector<std::function<bool(discpp::Context)>>& requirements);

		virtual void CommandBody(const discpp::Context& ctx);
		virtual bool CanRun(const discpp::Context& ctx);

		std::function<void(discpp::Context)> function = nullptr;
		std::string name; /**< Name of the current command. Ex: "ping"*/
		std::string description; /**< Description of the current command. Ex: "replies pong!" */
		std::vector<std::string> hint_args; /**< Arguments of the current command. Ex: @user */
		std::vector<std::function<bool(discpp::Context)>> requirements;
	};
}

#endif