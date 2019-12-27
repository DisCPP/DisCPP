#include <bot.h>
#include <context.h>
#include <command_handler.h>
#include <channel.h>

#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
	std::ifstream token_file("D:\\Code\\cpp\\discord\\discordpp\\token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discord::Bot bot{ token, "!" };

	discord::RegisterCommand("test", "hehe", { }, [](discord::Context ctx) {
		ctx.channel.Send("Message was created by: " + ctx.message.author.username);
	}, {});

	discord::RegisterCommand("channels", "hehe", { }, [](discord::Context ctx) {
		std::string finished = "";
		for (discord::Channel channel : ctx.bot->channels) {
			finished += " " + channel.name + " ";
		}
		ctx.channel.Send(finished);
	}, {});

	discord::RegisterCommand("edit-channel-name", "hehe", {"<name>"}, [](discord::Context ctx) {
		std::string new_name = discord::CombineVectorWithSpaces(ctx.arguments);
		ctx.channel.Modify( { discord::ModifyChannelValue::NAME, new_name } );
		ctx.Send("Changed name to: **" + new_name + "**");
	}, {});

	discord::RegisterCommand("delete-channel", "hehe", { }, [](discord::Context ctx) {
		ctx.channel.Delete();
	}, {});

	discord::RegisterCommand("find-message", "hehe", { "<message-id>" }, [](discord::Context ctx) {
		discord::snowflake message_id = discord::ToSnowflake(ctx.arguments[0]);
		discord::Message message = ctx.channel.FindMessage(message_id);
		ctx.Send("Found message! The text is: **" + message.content + "**");
	}, {});

	return bot.Run();
}