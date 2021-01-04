#include "command_server_info.h"
#include "precondition_require_guild.h"
#include <discpp/color.h>
#include <discpp/command.h>
#include <discpp/command_handler.h>

GuildInfoCommand::ServerinfoCommand() : discpp::Command("guildinfo") {
	this->description = "Displays helpful server information";
	this->hint_args = { "" };
}

bool GuildInfoCommand::CanRun(discpp::Context ctx) {
	return Precondition::RequireGuild(ctx);
}

void GuildInfoCommand::CommandBody(discpp::Context ctx) {
	std::shared_ptr<discpp::Guild> guild = ctx.message.guild;
	discpp::EmbedBuilder* embed = new discpp::EmbedBuilder("", "", 0x3498DB);
	embed->SetThumbnail(guild->GetIconURL());
	std::string iconURL = guild->GetIconURL();
	embed->AddField("Server Name:", guild->name, true);
	embed->AddField("Server ID:", std::to_string(guild->id), true);
	embed->AddField("Created At:", guild->GetFormattedCreatedAt(), true);
	std::string ownerId = std::to_string(guild->owner_id);
	embed->AddField("Owner:", "<@!" + ownerId + ">", true);
	embed->AddField("Members:", std::to_string(guild->member_count), true);
	embed->AddField("Server Region:", guild->region, true);
	embed->AddField("Verification Level:", std::to_string(guild->verification_level));
	ctx.channel.Send("", false, embed);
}