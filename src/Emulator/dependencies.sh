#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip python3-tomli libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev
    sudo apt-get install clang-16
elif [[ "$OSTYPE" == "darwin"* ]]; then
    brew install llvm@16
    export PATH="/opt/homebrew/opt/llvm@16/bin:$PATH"
    export CC="/opt/homebrew/opt/llvm@16/bin/clang"
    export CXX="$CC++"
    export LDFLAGS="$LDFLAGS -L/opt/homebrew/opt/llvm@16/lib"
    export CPPFLAGS="$CPPFLAGS -I/opt/homebrew/opt/llvm@16/include"
fi