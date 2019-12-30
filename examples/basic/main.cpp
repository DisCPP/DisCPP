#include <bot.h>
#include <context.h>
#include <command_handler.h>
#include <channel.h>
#include <embed_builder.h>

#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
	std::ifstream token_file("D:\\Code\\cpp\\discord\\discordpp\\token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discord::Bot bot{ token, "!" };

	discord::RegisterCommand("ping", "", { }, [&](discord::Context ctx) {
		ctx.channel.TriggerTypingIndicator();
		std::this_thread::sleep_for(std::chrono::milliseconds(750));
		ctx.Send("Pong!");
	}, {});

	return bot.Run();
}