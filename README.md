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
# To avoid CMake complaining about glew not getting any sources, issue the following:
make -C deps/glew/auto

# Currently there's no special flag for graphics because only OpenGL is planned for (Vulkan is a pain)
# Also, sorry json-c and libarchive add so much configure time, I can't do anything about it if I
#  want them up-to-date (which I do)
cmake -S. -Bbuild
cmake --build build # If you used the Makefile or Ninja generator, add on "-j`nproc`"
build/purpl-demo
```

## Libraries I plan on using
These are some really awesome libraries that have served me well in my past attempts (all credit goes to their authors, who have made my task much easier, so thanks for that. You can find all the licenses in their respective folders):
- [cglm](https://github.com/recp/cglm) is a great math library for doing linear algebra in C (good for graphics in particular)
- [GLEW](https://github.com/nigels-com/glew) is an amazing OpenGL loader
- [json-c](https://github.com/json-c/json-c) is good for parsing JSON
- [libarchive](https://github.com/libarchive/libarchive) is a super awesome archive reader that I intend to use for embedding assets into programs (still figuring that out)
- [stb](https://github.com/nothings/stb) is a collection of wonderful open source libraries contained in single header files
