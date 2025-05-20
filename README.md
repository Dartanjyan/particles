# Particles
Nobody is going to read this so I am not adding full instructions on how to install libraries. Use Google.
## Build on Linux
### Dependencies
`
SDL2
make
cmake
`
To build and install chipmunk library run
```shell
./scripts/build-chipmunk.sh
```
On arch linux there's a `chipmunk` package in AUR but it is built there in Release mode wich doesn't work for me.
`chipmunk-devel` on Fedora and `libchipmunk-dev` on Debian should be fine.
### Compiling
```shell
make
```
Binary will appear in `build` directory. To build and run there's a target `run` in `Makefile`:
```shell
make run
```
I use Arch btw and had the problem: `./build/particles: error while loading shared libraries: libchipmunk.so.7: cannot open shared object file: No such file or directory`. To fix this, I had to set up a `LD_LIBRARY_PATH` variable:
```shell
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
## Build on Linux for Windows
### Dependencies
`
mingw-w64
cmake
make
zip
`
### Compiling
Warning: be careful with the `$(nproc)` used in `$MAKEFLAGS` variable.
```shell
export MAKEFLAGS=$(nproc)
HOST=Windows ./scripts/build-chipmunk.sh
HOST=Windows ./scripts/build-sdl2.sh
```
These scripts will build dll's for Windows. Then build the project:
```
make windows
```
After this, `particles.zip` must appear in `build` directory. On Windows machine UNPACK this before launching. Don't launch it straightly from the archive.
