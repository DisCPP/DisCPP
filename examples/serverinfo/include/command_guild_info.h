#ifndef REQUIREGUILD_GUILDINFO_COMMAND_H
#define REQUIREGUILD_GUILDINFO_COMMAND_H

#include <discpp/command.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>

class GuildInfoCommand : public discpp::Command {
public:
	GuildInfoCommand();
	bool CanRun(discpp::Context ctx);
	void CommandBody(discpp::Context ctx) override;
};

#endif