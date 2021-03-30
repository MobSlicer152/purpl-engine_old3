# Purpl Engine 3.0
This is my third attempt at making a game engine. Having learned from those attempts, I think this one will be good. My goal is to at some point have the facilities necessary to create a game. The engine will be written in C, because C is for superior beings.

## How to build the engine
To build the engine, check that you have the right OpenGL development libraries (it varies between systems, Windows SDK on Windows and GLX and related on Linux (because I don't really know much about Wayland, sorry)). Then, clone the repo with
```sh
# Don't forget the --recursive!!!1!
git clone --recursive https://github.com/MobSlicer152/purpl-engine.git
```
and build like this
```sh
# Also note that GLEW has to be downloaded so that nothing has to be generated, with the downside that
#  a network connection is needed
cmake -S. -Bbuild
cmake --build build # If you used the Makefile or Ninja generator, add on "-j`nproc`" to make things go faster (POSIX shell w/ coreutils or similar only)
build/purpl-demo
```

## Libraries I plan on using
These are some really awesome libraries that have served me well in my past attempts (all credit goes to their authors, who have made my task much easier, so thanks for that. You can find all the licenses in their respective folders):
- [cglm](https://github.com/recp/cglm) is a great math library for doing linear algebra in C (good for graphics in particular)
- [GLEW](https://github.com/nigels-com/glew) is an amazing OpenGL loader
- [json-c](https://github.com/json-c/json-c) is good for parsing JSON files, which are very human readable/writable and therefore used frequently by the engine.
- [libarchive](https://github.com/libarchive/libarchive) is a great library for parsing archives
- [SDL 2](https://libsdl.org) is an amazing library for abstracting window creation, audio, and input between platforms
- [stb](https://github.com/nothings/stb) is a collection of wonderful open source libraries contained in single header files

<sub>Sorry about the profanity. Microsoft is annoying.</sub>
