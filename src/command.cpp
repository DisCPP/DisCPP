#include "log.h"
#include "command.h"
#include "command_handler.h"

discpp::Command::Command(const std::string& name) : name(name) { }

discpp::Command::Command(const std::string& name, const std::string& desc, const std::vector<std::string>& hint_args, const std::function<void(discpp::Context)>& function, const std::vector<std::function<bool(discpp::Context)>>& requirements) : Command() {
	this->name = name;
	this->description = desc;
	this->hint_args = hint_args;
	this->function = function;
	this->requirements = requirements;
}
discpp::Command::Command(const std::string& name, const std::string& desc, const std::function<void(discpp::Context)>& function, const std::vector<std::function<bool(discpp::Context)>>& requirements) : Command() {
	this->name = name;
	this->description = desc;
	this->function = function;
	this->requirements = requirements;
}

void discpp::Command::CommandBody(discpp::Context ctx) {
    if (function == nullptr) {
        ctx.shard.client.logger->Info("CommandBody for command '" + this->name + "' is not set!");
    } else {
        function(ctx);
    }
}

bool discpp::Command::CanRun(discpp::Context ctx) {
	bool requires = true;
	for (auto req_function : requirements) {
		requires = requires && req_function(ctx);
	}

	return requires;
}

void discpp::Command::SubCommandHandler(discpp::Context ctx) {
    auto argument_vec = ctx.arguments;
    if (argument_vec.empty()) {
        if (!this->CanRun(ctx)) return;
        this->CommandBody(ctx);
    }

    auto found_command = registered_commands.find(argument_vec[0]);

    if (found_command == registered_commands.end()) return;
    argument_vec.erase(argument_vec.begin()); // Erase the command from the arguments

    std::shared_ptr<discpp::Member> member = ctx.message.member;

    std::string remainder = "";
    if (!argument_vec.empty()) remainder = CombineStringVector(argument_vec);

    Context context = Context(ctx.shard, ctx.message.channel, member, ctx.message, ctx.remainder, argument_vec);

    // Check if the command can run
    if (!found_command->second->CanRun(context)) return;
    found_command->second->CommandBody(context);
}

void discpp::Command::AddAlias(const std::string& name) {
    this->aliases.push_back(name);
}

discpp::SubCommand::SubCommand(const std::string &name) {
    this->name = name;
}

discpp::SubCommand::SubCommand(const std::string &name, const std::string &desc, const std::vector<std::string> &hint_args, const std::function<void(discpp::Context)> &function, const std::vector<std::function<bool(discpp::Context)>> &requirements) {
    this->name = name;
    this->description = desc;
    this->hint_args = hint_args;
    this->function = function;
    this->requirements = requirements;
}