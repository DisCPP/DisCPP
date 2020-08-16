/*
	Basic bot showing off commands
*/

#include <discpp/client.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>

// Events
#include <discpp/event_handler.h>
#include <discpp/events/ready_event.h>
#include <discpp/events/guild_member_add_event.h>
#include <discpp/events/channel_pins_update_event.h>
#include <discpp/client_config.h>

#include "ping_command.h"

int main(int argc, const char* argv[]) {
	std::ifstream token_file("token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	std::cout << "Read token: " << token << std::endl;

	discpp::ClientConfig* config = new discpp::ClientConfig({"!"});
	discpp::Client bot{ token, config }; // Token, config

	PingCommand(); // This runs the constructor which will register the command.

	// I would recommend creating a class for the commands, you can check that in the examples folder
	// But, you can still register a command like you did before
	discpp::Command("test", "Quick example of a quick command", {}, [](discpp::Context ctx) {
		ctx.Send("Quick new command handler test");
	}, {});

	// New event system
	bot.event_handler->RegisterListener<discpp::ReadyEvent>([&bot](discpp::ReadyEvent event) {
		std::cout << "Ready!" << std::endl
			<< "Logged in as: " << bot.client_user.username << "#" << bot.client_user.GetDiscriminator() << std::endl
			<< "ID: " << bot.client_user.id << std::endl << "-----------------------------" << std::endl;

		// Will show "Playing With Crashes!"
		discpp::Presence activity("With DisC++!", discpp::Activity::ActivityType::GAME, "online");
		bot.UpdatePresence(activity);
	});

	return bot.Run();
}