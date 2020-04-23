#ifndef REQUIREGUILD_GUILDINFO_COMMAND_H
#define REQUIREGUILD_GUILDINFO_COMMAND_H

#include <discpp/command.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>

class ServerinfoCommand : public discord::Command {
public:
	ServerinfoCommand();
	virtual bool CanRun(discord::Context ctx) override;
	virtual void CommandBody(discord::Context ctx) override;
};

#endif