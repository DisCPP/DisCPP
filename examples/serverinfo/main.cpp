/*
	Basic bot showing off preconditions
*/

#include <discpp/client.h>
#include <discpp/client_config.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>

#include <discpp/event_handler.h>
#include <discpp/events/ready_event.h>

#include "CommandServerinfo.h"

int main(int argc, const char* argv[]) {
	std::ifstream token_file("token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discpp::ClientConfig* config = new discpp::ClientConfig({"!"});
	discpp::Client bot { token, config }; // Token, config

	bot.command_handler->RegisterCommand<ServerinfoCommand>();
	
	// ready event
    bot.event_handler->RegisterListener<discpp::ReadyEvent>([&] (const discpp::ReadyEvent& event) {
		std::cout << "Ready!" << std::endl; 
	});

	return bot.Run();
}