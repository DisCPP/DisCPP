<h1 align="center">DiscordPP</h1>
<p align="center">
  <a href="https://ci.appveyor.com/project/SeanOMik/discordpp">
    <img src="https://ci.appveyor.com/api/projects/status/8e23w925eqahmub6?svg=true">
  </a>
</p>
<p align="center">
Discord API Wrapper Written in C++  
</p>

## DiscordPP
DiscordPP is a Discord API wrapper written in C++. This is one of the few that can compile on Windows and Linux. Its also focused on being multi threaded so commands and event listeners are ran on seperate threads, this means you dont have to worry about slowing the bot down with a certain command.

## Dependencies
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [cpr](https://github.com/whoshuu/cpr)
- [cpprestsdk](https://github.com/microsoft/cpprestsdk.git)
- [Boost Serialization](https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/index.html)

## Contributing
Please follow [Google's styling guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming convention.

## Building
1. Install vcpkg onto the root of your C drive.
2. Install dependencies by running command: `vcpkg install nlohmann-json cpr cpprestsdk boost-serialization`.
3. Then run `vcpkg integrate install`.
    * Should get an output similar to: `"-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"`.
    * Save the directory that was given (Ex. `C:/vcpkg/scripts/buildsystems/vcpkg.cmake`).
4. Clone this repository.
5. Open the CMake project in Visual Studio.
    * On the welcome screen under the create a project or open project buttons, click `Continue without code`
    * Open the CMakeLists.txt by `File > Open > Cmake`
    * Browse to where you cloned the repository and double click on CMakeLists.txt file.
6. Go into CMake settings and set the CMake toolchain to the directory you saved above.
    * Click on `Project > CMake Settings`
    * Scroll to the CMake toolchain file text box and enter the directory you saved.
## Setting up a Bot Project
First follow the [building](#Building) steps above to make sure DiscordPP will compile.
1. Currently you need to create a thirdparty folder in the root of your bot project directory.
2. Inside the thirdparty folder, clone this repository.
3. Open your CMake file and add `add_subdirectory(thirdparty/DiscordPP)` near the top.
4. Where ever you link your libraries, add this: `target_link_libraries(main PRIVATE discordpp)` (usually at, or near, the end of the file)
5. Your finished CMakefile should be similar to this:
```cmake
#CMakeLists.txt
cmake_minimum_required(VERSION 3.6)
project(bot)

add_subdirectory(thirdparty/DiscordPP)

add_executable(main src/main.cpp)

target_include_directories(main PUBLIC include)

file(GLOB_RECURSE source_list src/*.cpp)
target_sources(main PRIVATE ${source_list})

target_link_libraries(main PRIVATE discordpp)
```
6. Edit your cmake settings to reflect how it was edited in [building](#Building) steps.
7. You're done, enjoy!

## Todo:
- [x] Finish tons of unimplemented methods in certain classes.
- [ ] Convert `std::bind` to lambdas.
- [ ] Convert the default command handler to use command classes.

## Documentation
Currently the only documentation is from Doxygen; its currently completely up to date. To generate documentation, install Doxygen and run doxygen in the root DiscordPP directory.

## Examples
There may be more inside the [Examples](examples) folder.
```cpp
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

	discord::Bot bot{ token, "!" }; // Token, command prefix.

	PingCommand(); // This runs the constructor which will register the command.

	// I would recommend creating a class for the commands, you can check that in the examples folder
	// But, you can still register a command like you did before
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
```

### License 
See [LICENSE](LICENSE.md)
