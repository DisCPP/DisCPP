# DiscordPP
Currently it only connects to the discord gateway but crashes with undefined behavior when trying to send anything.

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
