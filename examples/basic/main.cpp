#include <discordpp/bot.h>
#include <discordpp/context.h>
#include <discordpp/command_handler.h>
#include <discordpp/channel.h>
#include <discordpp/activity.h>
#include <discordpp/command.h>
// Events
#include <discordpp/event_handler.h>
#include <discordpp/events/ready_event.h>
#include <discordpp/events/guild_member_add_event.h>
#include <discordpp/events/channel_pins_update_event.h>

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

	// New event system
	discord::EventHandler<discord::ReadyEvent>::RegisterListener([&bot](discord::ReadyEvent event)->bool {
		std::cout << "Ready!" << std::endl
			<< "Logged in as: " << bot.bot_user.username << "#" << bot.bot_user.discriminator << std::endl
			<< "ID: " << bot.bot_user.id << std::endl << "-----------------------------" << std::endl;

		// Will show "Playing With Crashes!"
		discord::Activity activity = discord::Activity("With Crashes!", discord::presence::ActivityType::GAME, discord::presence::Status::idle);
		bot.UpdatePresence(activity);

		return false;
	});

	discord::EventHandler<discord::GuildMemberAddEvent>::RegisterListener([](discord::GuildMemberAddEvent event)->bool {
		discord::Channel channel((discord::snowflake) "638156895953223714");

		channel.Send("Welcome <@" + event.member.user.id + ">, hope you enjoy!");

		return false;
	});

	discord::EventHandler<discord::ChannelPinsUpdateEvent>::RegisterListener([](discord::ChannelPinsUpdateEvent event)->bool {
		event.channel.Send("Detected a pin update!");

		return false;
	});

	return bot.Run();
}