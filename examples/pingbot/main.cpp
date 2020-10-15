/*
	Basic ping bot
*/

#include <discpp/client.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>
#include <discpp/client_config.h>
#include <discpp/event_handler.h>
#include <discpp/events/ready_event.h>

int main(int argc, const char* argv[]) {
    discpp::ClientConfig config({"!"});
	// Make sure to replace `TOKEN HERE` with your actual token
	discpp::Client client{ "TOKEN HERE", config }; // Token, config

	// For more complex commands, create a class for each one.
    client.command_handler->RegisterCommand<discpp::Command>("ping", "", [](discpp::Context ctx) {
        ctx.Send("Pong!");
	});

    // This notifies you in the console when the client becomes ready to receive and send requests
    client.event_handler->RegisterListener<discpp::ReadyEvent>([&] (const discpp::ReadyEvent& event) {
        client.logger->Info("Ready!");
    });

	return client.Run();
}