#!/bin/bash

###
# This script builds chipmunk for Windows if you run it with an env variable
# $HOST set to Windows, like:
# HOST=Windows ./scripts/build-chipmunk.sh
#
# If $HOST is not "Windows" then it will build and install this library for linux with sudo.
###

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
CHIPMUNK_DIR="$(realpath ${SCRIPT_DIR}/..)/3rdparty/chipmunk"
SRC_DIR="${CHIPMUNK_DIR}/src"
INSTALL_DIR="${CHIPMUNK_DIR}/install"

if [[ $HOST == Windows ]]; then
    WINDOWS_PARAMS=" \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
        -DCMAKE_EXE_LINKER_FLAGS=\"-lpthread\" \
        -DCMAKE_SHARED_LINKER_FLAGS=\"-lpthread\" \
        -DBUILD_SHARED=OFF \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    "
else
    WINDOWS_PARAMS=" \
        -DBUILD_SHARED=ON \
    "
fi

mkdir -pv "${CHIPMUNK_DIR}"

if [[ ! -d $SRC_DIR ]] then
    git clone --depth=1 https://github.com/slembcke/Chipmunk2D ${SRC_DIR}
fi

cd ${SRC_DIR}
rm -rf build
mkdir -pv build
cd build

cmake .. \
    --fresh \
    -DBUILD_DEMOS=OFF \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNDEBUG=ON \
    $WINDOWS_PARAMS

make
if [[ $HOST == Windows ]]; then
    make install
else
    echo -e "\nInstalling Chipmunk library."
    sudo make install
fi

# rm -rf ${SRC_DIR}
echo ------------------------------------------------------------
echo "Chipmunk2D built and installed to ${INSTALL_DIR}"
