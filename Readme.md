<h1 align="center">DisC++</h1>

<p align="center">
<a href="https://discord.gg/reQbbtc"><img src="https://discord.com/api/guilds/699405108982579300/widget.png?style=shield"></a>
<br>
Simplified, but feature rich Discord API wrapper written in modern C++.
</p>

## DisC++
DisC++ is a simplified, but feature rich Discord API wrapper written in modern C++. It can run on Linux and Windows. Its also focused on being multi threaded so commands and event listeners are ran on seperate threads, this means you dont have to worry about slowing the bot down with a certain command.

## State
This library is ready to use but is still in development so you may encounter a few issues, if you do, please submit an issue!

<table align="center">
  <tr>
    <th>Master</th>
    <th>Development</th> 
  </tr>
  <tr>
    <td align="center"><a href="https://dev.azure.com/seanomik/DisCPP/_build">
    <img src="https://dev.azure.com/seanomik/DisCPP/_apis/build/status/DisCPP.DisCPP?branchName=master">
  </a></td>
    <td align="center"><a href="https://dev.azure.com/seanomik/DisCPP/_build">
    <img src="https://dev.azure.com/seanomik/DisCPP/_apis/build/status/DisCPP.DisCPP?branchName=development">
  </a></td>
  </tr>
</table>

## Dependencies
- [RapidJSON](https://github.com/Tencent/rapidjson)
- [cpr](https://github.com/whoshuu/cpr)
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)

## Contributing
Please follow [Google's styling guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming convention.

## Building
1. Install vcpkg onto the root of your C drive.
2. Install dependencies by running command: `vcpkg install rapidjson cpr openssl --triplet x64-[windows/linux]`.
    * Replace [windows/linux] with your operating system.
3. Then run `vcpkg integrate install`.
    * Should get an output similar to: `"-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"`.
    * Save the directory that was given (Ex. `C:/vcpkg/scripts/buildsystems/vcpkg.cmake`).
4. Clone this repository.
    * `git clone https://github.com/DisCPP/DisCPP.git`
    * `git submodule update --init`
5. Open the CMake project in Visual Studio.
    * On the welcome screen under the create a project or open project buttons, click `Continue without code`
    * Open the CMakeLists.txt by `File > Open > Cmake`
    * Browse to where you cloned the repository and double click on CMakeLists.txt file.
6. Go into CMake settings and set the CMake toolchain to the directory you saved above.
    * Click on `Project > CMake Settings`
    * Scroll to the CMake toolchain file text box and enter the directory you saved.
7. While inside CMake settings, make sure your compiling in x64-Debug.
	* Scroll to `Toolset` and set it to a x64 compiler of your choosing (ex: msvc_x64)
	* Only use MSVC on Windows!
Note: If you get any errors during this process, join our Discord and we can help you!
## Setting up a Bot Project
First follow the [building](#Building) steps above to make sure Disc++ will compile.
Note: If you get any errors during this process, join our Discord and we can help you!
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

## Examples
There may be more inside the [Examples](examples) folder.

## Todo:
- [x] Finish tons of unimplemented methods in certain classes.
- [x] Convert the default command handler to use command classes.
- [x] Convert the event system to use a more object oriented system, similar to the new command handler.
- [x] Make sure all endpoints are implemented and add them if they aren't.
- [ ] Voice websocket connection.
- [ ] Sending audio through a voice connection.
- [ ] Make the audit log cleaner and easier to use.

## Documentation
Currently the only documentation is from Doxygen; its currently completely up to date. You can view it [here](https://discpp.github.io/). If you want to generate documentation you need to install Doxygen and run doxygen in the root DisCPP directory and run the terminal command: ```doxygen Doxyfile```

### License 
See [LICENSE](LICENSE.md)
