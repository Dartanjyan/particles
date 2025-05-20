# Particles
Nobody is going to read this so I am not adding full instructions on how to install libraries. Use Google.
## Build on Linux
### Dependencies
`
SDL2
chipmunk
`
### Compiling
To build, use `make`
## Build on Linux for Windows
### Dependencies
`
mingw-w64
cmake
zip
`
### Compiling
Warning: be careful with the `$(nproc)` used in `$MAKEFLAGS` variable.
```shell
export MAKEFLAGS=$(nproc)
bash ./scripts/build-chipmunk.sh
bash ./scripts/build-sdl2.sh
```
These scripts will build dll for Windows
```
make windows
```
