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
        /**
         * @brief Constructs a discpp::Command
         *
         * ```cpp
         *      discpp::Command("test", "Quick example of a quick command", {}, [](discpp::Context ctx) {
         *			ctx.Send("Quick new command handler test");
         *		}, {
         *			// Preconditions
         *		});
         * ```
         *
         * @param[in] name Command name
         * @param[in] desc The command description
         * @param[in] hint_args Command argument hints
         * @param[in] function The function that is executed when the command is triggered
         * @param[in] requirements A list of preconditions for the command.
         *
         * @return discpp::Command, this is a constructor.
         */
		Command(const std::string& name, const std::string& desc, const std::vector<std::string>& hint_args, const std::function<void(discpp::Context)>& function, const std::vector<std::function<bool(discpp::Context)>>& requirements);

        /**
         * @brief The method that is executed when the command is triggered if overrided.
         *
         * @param[in] ctx The command context filled with information about where this command is being executed.
         *
         * @return void
         */
        virtual void CommandBody(discpp::Context ctx);

        /**
         * @brief This method checks if the requirements of the command allowes it to run.
         *
         * This can be overrided if you extend from this class.
         *
         * @param[in] ctx The command context filled with information about where this command is being executed.
         *
         * @return void
         */
        virtual bool CanRun(discpp::Context ctx);

		std::function<void(discpp::Context)> function = nullptr;
		std::string name; /**< Name of the current command. Ex: "ping"*/
		std::string description; /**< Description of the current command. Ex: "replies pong!" */
		std::vector<std::string> hint_args; /**< Arguments of the current command. Ex: @user */
		std::vector<std::function<bool(discpp::Context)>> requirements;
	};
}

#endif