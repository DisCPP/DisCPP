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

	discord::RegisterCommand("test", "", { }, [&](discord::Context ctx) {
		try {
			discord::EmbedBuilder embed("test title", "description", 0x08ff3d);
			ctx.Send(embed);
		} catch (std::runtime_error & e) {
			std::cout << "ERROR: " << e.what() << std::endl;
		}
	}, {});

	return bot.Run();
}