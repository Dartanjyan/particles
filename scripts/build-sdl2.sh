#!/bin/bash
set -e

SDL_DIR="$(realpath .)/3rdparty/SDL"
SRC_DIR="$(SDL_DIR)/src"
INSTALL_DIR="$(SDL_DIR)/install"

mkdir -pv $SDL_DIR

if [[ ! -d $SRC_DIR ]]; then
    git clone --depth=1 https://github.com/libsdl-org/SDL.git -b SDL2 $SDL_DIR
fi

cd $SDL_DIR
mkdir build
cd build
../configure --prefix=$INSTALL_DIR

echo -e "No i teraz trzeba zabildować\n"

