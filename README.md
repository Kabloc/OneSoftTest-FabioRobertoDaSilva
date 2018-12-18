# OneSoftTest-FabioRobertoDaSilva

OneSoft C++ Coding Exercise: Simple Server

External dependencies is:
* [Boost Library](https://www.boost.org/users/history/version_1_66_0.html) - Version 1.66.0

## Build instructions

### Linux

To build execute (from project root folder):
```
> mkdir build
> cd build
> cmake ..
> cmake --build . --target all
```
Alternatively, the last step can be performed running make directly:
```
> make
```

To run application execute (from build folder):
```
> ./neg_file_server <market_file> [port]
```

Tested with:
* CMake 3.10.2
* g++ (Ubuntu 7.3.0-16ubuntu3) 7.3.0

### Windows

To build execute (from project root folder):
```
> mkdir build
> cd build
> cmake ..
> cmake --build . --target ALL_BUILD --config Debug
```
Alternatively, the last step can be performed with Visual Studio. Solution file (.sln) is generated by "cmake .." command.

To run application execute (from build folder):
```
> Debug\neg_file_server.exe <market_file> [port]
```

Tested with:
* CMake 3.11.10
* Microsoft Visual C++ 2017 Version 15.7.1
