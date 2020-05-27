#include "command.h"
#include "command_handler.h"

discpp::Command::Command(const std::string& name) : name(name) {
	discpp::registered_commands.insert(std::make_pair(name, this));
}

discpp::Command::Command(const std::string& name, const std::string& desc, const std::vector<std::string>& hint_args, const std::function<void(discpp::Context)>& function, const std::vector<std::function<bool(discpp::Context)>>& requirements) : Command() {
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

	this->name = name;
	this->description = desc;
	this->hint_args = hint_args;
	this->function = function;
	this->requirements = requirements;

	discpp::registered_commands.insert(std::make_pair(name, this));
}

void discpp::Command::CommandBody(const discpp::Context& ctx) {
	/**
	 * @brief The method that is executed when the command is triggered if overrided.
	 *
	 * @param[in] ctx The command context filled with information about where this command is being executed.
	 *
	 * @return void
	 */

	if (function == nullptr) {
		std::cout << "Make sure to override the \"CommandBody(discpp::Context)\" method to get an actual command body for: \"" + name + "\"" << std::endl;
	} else {
		function(ctx);
	}
}

bool discpp::Command::CanRun(const discpp::Context& ctx) {
	/**
	 * @brief This method checks if the requirements of the command allowes it to run.
	 *
	 * This can be overrided if you extend from this class.
	 *
	 * @param[in] ctx The command context filled with information about where this command is being executed.
	 *
	 * @return void
	 */

	bool requires = true;
	for (auto req_function : requirements) {
		requires = requires && req_function(ctx);
	}

	return requires;
}
