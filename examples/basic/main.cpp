#include <bot.h>
#include <context.h>
#include <command_handler.h>
#include <channel.h>
#include <embed_builder.h>
#include <events.h>
#include <activity.h>

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

	bot.HandleEvent<discord::events::ready>([&bot]() {
		std::cout << "Ready!" << std::endl
				  << "Logged in as: " << bot.bot_user.username << "#" << bot.bot_user.discriminator << std::endl
				  << "ID: " << bot.bot_user.id << std::endl
				  << "-----------------------------" << std::endl;

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