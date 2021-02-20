# purpl-engine
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
cmake -S. -Bbuild
cmake --build build
build/purpl-demo
```

## Libraries I plan on using
These are some really awesome libraries that have served me well in my past attempts (all credit goes to their authors, who have made my task much easier. You can find all the licenses in their respective folders):
- [cglm](https://github.com/recp/cglm) is a great math library for doing linear algebra in C (good for graphics in particular)
- [GLEW](https://github.com/nigels-com/glew) is an amazing OpenGL loader
- [json-c](https://github.com/json-c/json-c) is good for parsing JSON
- [libarchive](https://github.com/libarchive/libarchive) is a super awesome archive reader that I intend to use for embedding assets into programs (still figuring it out)
- [stb](https://github.com/nothings/stb) is a collection of wonderful open source libraries contained in single header files
