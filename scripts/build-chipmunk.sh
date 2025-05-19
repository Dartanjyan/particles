#!/bin/bash

###
# This script builds chipmunk for Windows. 
# If you want to install for linux, then search it on repositories.
#
# AUR: chipmunk
# Fedora: chipmunk-devel
# Debian (checked): libchipmunk-dev
###

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
CHIPMUNK_DIR="$(realpath ${SCRIPT_DIR}/..)/3rdparty/chipmunk"
SRC_DIR="${CHIPMUNK_DIR}/src"
INSTALL_DIR="${CHIPMUNK_DIR}/install"

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
    -DCMAKE_SYSTEM_NAME=Windows \
    -DBUILD_DEMOS=OFF \
    -DBUILD_SHARED=OFF \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNDEBUG=ON \
    -DCMAKE_EXE_LINKER_FLAGS="-lpthread" \
    -DCMAKE_SHARED_LINKER_FLAGS="-lpthread"

make
make install

# rm -rf ${SRC_DIR}
echo ------------------------------------------------------------
echo "Chipmunk2D built and installed to ${INSTALL_DIR}"
