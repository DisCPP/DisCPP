<h1 align="center">DiscordPP</h1>
<p align="center">
  <a href="https://ci.appveyor.com/project/SeanOMik/discordpp">
    <img src="https://ci.appveyor.com/api/projects/status/8e23w925eqahmub6?svg=true">
  </a>
</p>
<p align="center">
Discord API Wrapper Written in C++  
</p>

## State
Good to use for some smaller projects. Not sure how it would work on larger scale projects.

## Contributing
Please follow [Google's styling guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming convention.

## Dependencies
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [cpr](https://github.com/whoshuu/cpr)
- [cpprestsdk](https://github.com/microsoft/cpprestsdk.git)

## Building
- Install vcpkg onto the root of your C drive (if you dont you'll need to modify the cmake file).
- Install dependencies by running command: `vcpkg install nlohmann-json cpr cpprestsdk`.
- Then run `vcpkg integrate install`.
- Open the CMake project in Visual Studio.

## Examples
There may be more inside the [Examples](examples) folder.
```cpp
#include <bot.h>
#include <context.h>
#include <command_handler.h>
#include <channel.h>
#include <events.h>
#include <activity.h>

#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
	std::ifstream token_file("D:\\Code\\cpp\\discord\\discordpp\\token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discord::Bot bot{ token, "!" }; // Token, command prefix

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

	return bot.Run();
}
```

### License 
See [LICENSE](LICENSE.md)
