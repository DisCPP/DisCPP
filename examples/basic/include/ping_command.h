#ifndef DISCORDPP_BASIC_EXAMPLE_PING_COMMAND_H
#define DISCORDPP_BASIC_EXAMPLE_PING_COMMAND_H

#include <discordpp/command.h>
#include <discordpp/context.h>

class PingCommand : public discord::Command {
public:
	PingCommand();
	void CommandBody(discord::Context ctx);
};

#endif