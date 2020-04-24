#include "ping_command.h"
#include <discpp/command_handler.h>

PingCommand::PingCommand() : discpp::Command("ping") {
	this->description = "Just a quick test command";
}

void PingCommand::CommandBody(discpp::Context ctx) {
	ctx.channel.TriggerTypingIndicator();
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	ctx.Send("Pong!");
}