<h1 align="center">DisC++</h1>

<p align="center">
<a href="https://discord.gg/reQbbtc"><img src="https://discord.com/api/guilds/699405108982579300/widget.png?style=shield"></a>
<br>
Simplified, but feature rich Discord API wrapper written in modern C++.
</p>

## DisC++
DisC++ is a simplified, but feature rich Discord API wrapper written in modern C++. It can run on Linux and Windows. Its also focused on being multi threaded so commands and event listeners are ran on seperate threads, this means you dont have to worry about slowing the bot down with a certain command.

## What makes DisC++ different?
* Low memory usage.
* Even though its very memory optimized, it doesn't loose any readability, usability or features.
* Asynchronous.
* Built in async command handler.
    * Commands are ran on a separate thread.
    * You can override the built in command handler if you decide to.
* Built in async event handler and listener.
    * Event listeners are dispatched on separate threads.
    * You can have several event listeners listening for the same event which are all ran on separate threads.

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

## [Wiki](https://github.com/DisCPP/DisCPP/wiki#getting-started)
Information on how to setup a project and how to compile this application will be found in the wiki.

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
Currently the only documentation is from Doxygen; its currently completely up to date. You can view it [here](https://github.com/DisCPP/DisCPP). If you want to generate documentation you need to install Doxygen and run doxygen in the root DisCPP directory and run the terminal command: ```doxygen Doxyfile```

### License 
See [LICENSE](LICENSE.md)
