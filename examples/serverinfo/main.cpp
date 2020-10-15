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
	discpp::ClientConfig config({"!"});
    // Make sure to replace `TOKEN HERE` with your actual token
	discpp::Client client { "TOKEN_HERE", config }; // Token, config

    client.command_handler->RegisterCommand<ServerinfoCommand>();

    client.event_handler->RegisterListener<discpp::ReadyEvent>([&] (const discpp::ReadyEvent& event) {
        client.logger->Info("Ready! (v0.1.0)");
        client.logger->Info("Logged in as: " + client.client_user.username + "#" + client.client_user.GetDiscriminator());
        client.logger->Info("ID: " + std::to_string(client.client_user.id));
        client.logger->Info(std::string(20, '-'));
	});

	return client.Run();
}