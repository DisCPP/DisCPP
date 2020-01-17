#include "command.h"
#include "command_handler.h"

discord::Command::Command(std::string name) {
	//discord::registered_commands.emplace(name, std::make_shared<Command>(*this));
	discord::registered_commands.insert(std::make_pair(name, std::make_shared<Command>(*this)));
}

discord::Command::Command(std::string name, std::string desc, std::vector<std::string> hint_args, std::function<void(discord::Context)> function, std::vector<std::function<bool(discord::Context)>> requirements) : Command() {
	this->name = name;
	this->description = desc;
	this->hint_args = hint_args;
	this->function = function;
	this->requirements = requirements;
}

void discord::Command::CommandBody(discord::Context ctx) {
	if (function == nullptr) {
		std::cout << "Make sure to override the \"CommandBody(discord::Context)\" method to get an actual command" << std::endl;
	} else {
		function(ctx);
	}
}

bool discord::Command::CanRun(discord::Context ctx) {
	//if (requirements.size() == 0) return true;
	
	bool requires = true;
	for (auto req_function : requirements) {
		requires = requires && req_function(ctx);
	}

	return requires;
}
