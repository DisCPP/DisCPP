#include <discordpp/bot.h>
#include <discordpp/context.h>
#include <discordpp/command_handler.h>
#include <discordpp/channel.h>
#include <discordpp/events.h>
#include <discordpp/activity.h>
#include <discordpp/command.h>

#include <iostream>
#include <fstream>

#include "ping_command.h"

int main(int argc, const char* argv[]) {
	std::ifstream token_file("token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discord::Bot bot{ token, "!" };

	PingCommand(); // This runs the constructor which would actually register the command

	// You can still register a command like you did before
	discord::Command("test", "Quick example of a quick command", {}, [](discord::Context ctx) {
		ctx.Send("Quick new command handler test");
	}, {});

	bot.HandleEvent<discord::events::ready>([&bot]() {
		std::cout << "Ready!" << std::endl
				  << "Logged in as: " << bot.bot_user.username << "#" << bot.bot_user.discriminator << std::endl
				  << "ID: " << bot.bot_user.id << std::endl << "-----------------------------" << std::endl;

		// Will show "Playing With Crashes!"
		discord::Activity activity = discord::Activity("With Crashes!", discord::presence::ActivityType::GAME, discord::presence::Status::idle);
		bot.UpdatePresence(activity);
	});

	bot.HandleEvent<discord::events::guild_member_add>([&bot](discord::Guild const guild, discord::Member const member) {
		discord::Channel channel((discord::snowflake) "638156895953223714");
		
		channel.Send("Welcome <@" + member.user.id + ">, hope you enjoy!");
	});

	bot.HandleEvent<discord::events::channel_pins_update>([&bot](discord::Channel const channel) {
		discord::Channel _channel = channel;
		_channel.Send("Detected a pin update!");
	});

	return bot.Run();
}