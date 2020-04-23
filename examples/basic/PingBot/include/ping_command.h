#ifndef DISCPP_BASIC_EXAMPLE_PING_COMMAND_H
#define DISCPP_BASIC_EXAMPLE_PING_COMMAND_H

#include <discpp/command.h>
#include <discpp/context.h>

class PingCommand : public discord::Command {
public:
	PingCommand();
	virtual void CommandBody(discord::Context ctx) override;
};

#endif