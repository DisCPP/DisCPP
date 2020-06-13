#ifndef REQUIREGUILD_GUILDINFO_COMMAND_H
#define REQUIREGUILD_GUILDINFO_COMMAND_H

#include <discpp/command.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>

class ServerinfoCommand : public discpp::Command {
public:
	ServerinfoCommand();
	bool CanRun(discpp::Context ctx);
	virtual void CommandBody(discpp::Context ctx) override;
};

#endif