#include "command_handler.h"
#include "utils.h"

void discord::FireCommand(discord::Bot* bot, discord::Message message) {
	if (!StartsWith(message.content, bot->prefix)) { //|| message.author.bot) {
		return;
	}

	std::vector<std::string> argument_vec = SplitString(message.content, ' ');
	if (!argument_vec.size()) return;

	auto found_command = registered_commands.find(argument_vec[0].substr(bot->prefix.size()));
	if (found_command == registered_commands.end()) return;

	auto command_name = argument_vec.front().erase(0, bot->prefix.size());
	auto command_function = registered_commands.at(command_name).function;
	argument_vec.erase(argument_vec.begin()); // Erase the command from the arguments

	auto found_member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [message](discord::Member a) { return message.author.id == a.user.id; });
	discord::Member member;

	if (found_member != discord::globals::bot_instance->members.end()) {
		member = *found_member;
	}

	Context context = Context(bot, message.channel, member, message, argument_vec);

	bool requirements = true;
	for (auto req_function : registered_commands.at(command_name).requirements) {
		requirements = requirements && req_function(context);
	}

	if (!requirements) {
		return; // Requirements not met
	}

	bot->DoFunctionLater(found_command->second.function, context);
}

void discord::RegisterCommand(std::string name, std::string description, std::vector<std::string> hint_args, std::function<void(Context)> function, std::vector<std::function<bool(Context)>> requirements) {
	Command command = { function, name, description, hint_args, requirements };
	registered_commands.emplace(name, command);
}
