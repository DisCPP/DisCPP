<h1 align="center">DisC++</h1>
<p align="center">
  <a href="https://dev.azure.com/seanomik/DisCPP/_build">
    <img src="https://dev.azure.com/seanomik/DisCPP/_apis/build/status/SeanOMik.DisCPP?branchName=master">
  </a>
</p>
<p align="center">
Discord API Wrapper Written in Modern C++ aimed towards ease of use.
</p>

## DisC++
DisC++ is a Discord API wrapper written in modern C++ aimed towards ease of use. This is one of the few that can compile on Windows and Linux. Its also focused on being multi threaded so commands and event listeners are ran on seperate threads, this means you dont have to worry about slowing the bot down with a certain command.

## Dependencies
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [cpr](https://github.com/whoshuu/cpr)
- [cpprestsdk](https://github.com/microsoft/cpprestsdk.git)
- [Boost Serialization](https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/index.html)

## Contributing
Please follow [Google's styling guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming convention.

## Building
1. Install vcpkg onto the root of your C drive.
2. Install dependencies by running command: `vcpkg install nlohmann-json cpr cpprestsdk cpprestsdk[websockets] boost-serialization`.
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
7. While inside CMake settings, make sure your compiling in x86-Debug.
	* Scroll to `Toolset` and set it to a x86 compiler of your choosing (ex: msvc_x86)
## Setting up a Bot Project
First follow the [building](#Building) steps above to make sure Disc++ will compile.
1. Currently you need to create a thirdparty folder in the root of your bot project directory.
2. Inside the thirdparty folder, clone this repository.
3. Open your CMake file and add `add_subdirectory(thirdparty/discpp)` near the top.
4. Where ever you link your libraries, add this: `target_link_libraries(main PRIVATE discpp)` (usually at, or near, the end of the file)
5. Your finished CMakefile should be similar to this:
```cmake
#CMakeLists.txt
cmake_minimum_required(VERSION 3.6)
project(bot)

add_subdirectory(thirdparty/discpp)

add_executable(main src/main.cpp)

target_include_directories(main PUBLIC include)

file(GLOB_RECURSE source_list src/*.cpp)
target_sources(main PRIVATE ${source_list})

target_link_libraries(main PRIVATE discpp)
```
6. Edit your cmake settings to reflect how it was edited in [building](#Building) steps.
7. You're done, enjoy!

## Todo:
- [x] Finish tons of unimplemented methods in certain classes.
- [x] Convert the default command handler to use command classes.
- [x] Convert the event system to use a more object oriented system, similar to the new command handler.

## Documentation
Currently the only documentation is from Doxygen; its currently completely up to date. To generate documentation, install Doxygen and run doxygen in the root DisCPP directory.

## Examples
There may be more inside the [Examples](examples) folder.
```cpp
#include <discpp/bot.h>
#include <discpp/context.h>
#include <discpp/command_handler.h>
#include <discpp/channel.h>
#include <discpp/activity.h>
#include <discpp/command.h>

// Events
#include <discpp/event_handler.h>
#include <discpp/events/ready_event.h>
#include <discpp/events/guild_member_add_event.h>
#include <discpp/events/channel_pins_update_event.h>

#include <iostream>
#include <fstream>

#include "ping_command.h"

int main(int argc, const char* argv[]) {
	std::ifstream token_file("token.txt", std::ios::out);
	std::string token;
	std::getline(token_file, token);

	discpp::Bot bot{ token, {"!"} }; // Token, command prefixes.

	PingCommand(); // This runs the constructor which will register the command.

	// I would recommend creating a class for the commands, you can check that in the examples folder
	// But, you can still register a command like you did before
	discpp::Command("test", "Quick example of a quick command", {}, [](discpp::Context ctx) {
		ctx.Send("Quick new command handler test");
	}, {});

	// New event system
	discpp::EventHandler<discpp::ReadyEvent>::RegisterListener([&bot](discpp::ReadyEvent event) {
		std::cout << "Ready!" << std::endl
			<< "Logged in as: " << bot.bot_user.username << "#" << bot.bot_user.discriminator << std::endl
			<< "ID: " << bot.bot_user.id << std::endl << "-----------------------------" << std::endl;

		// Will show "Playing With Crashes!"
		discpp::Activity activity = discpp::Activity("With Crashes!", discpp::presence::ActivityType::GAME, discpp::presence::Status::idle);
		bot.UpdatePresence(activity);
	});

	discpp::EventHandler<discpp::GuildMemberAddEvent>::RegisterListener([](discpp::GuildMemberAddEvent event) {
		discpp::Channel channel((discpp::snowflake) "638156895953223714");

		channel.Send("Welcome <@" + event.member.user.id + ">, hope you enjoy!");
	});

	discpp::EventHandler<discpp::ChannelPinsUpdateEvent>::RegisterListener([](discpp::ChannelPinsUpdateEvent event)->bool {
		event.channel.Send("Detected a pin update!");

		return false;
	});

	return bot.Run();
}
```

### License 
See [LICENSE](LICENSE.md)
