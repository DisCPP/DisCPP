#include "ping_command.h"
#include <discpp/command_handler.h>

PingCommand::PingCommand() : discord::Command("ping") {
	this->description = "Just a quick test command";
}

void PingCommand::CommandBody(discord::Context ctx) {
	ctx.channel.TriggerTypingIndicator();
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	ctx.Send("Pong!");
}