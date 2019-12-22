# DiscordPP
Currently it wont even connect to the Discord gateway servers so yeah, useless for now.

## Contributing
Please follow [Google's styling guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming convention.

## Dependencies
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [cpr](https://github.com/whoshuu/cpr)
- [ixwebsocket](https://github.com/machinezone/IXWebSocket)

## Buidling
- Install vcpkg onto the root of your C drive (if you dont you'll need to modify the cmake file).
- Install dependencies by running command: `vcpkg install nlohmann-json cpr zlib mbedtls ixwebsocket`.
- Then run `vcpkg integrate install`.
- Open the CMake project in Visual Studio.