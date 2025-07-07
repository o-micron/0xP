mkdir thirdparty
cd thirdparty

# 14.2.0
# 15.1.0
git clone https://github.com/riscv/riscv-gnu-toolchain riscv-gnu-toolchain
cd riscv-gnu-toolchain
    git checkout -f master
    git submodule update --init --recursive
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # we're going to use default gcc ..
        echo "all good"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # use clang as our compiler, assuming we have installed via homebrew ..
        brew install llvm@16
        export PATH="/opt/homebrew/opt/llvm@16/bin:$PATH"
        export CC="/opt/homebrew/opt/llvm@16/bin/clang"
        export CXX="$CC++"
        export LDFLAGS="$LDFLAGS -L/opt/homebrew/opt/llvm@16/lib"
        export CPPFLAGS="$CPPFLAGS -I/opt/homebrew/opt/llvm@16/include"
    fi
    rm -rf build32
    rm -rf install32
    mkdir build32
    mkdir install32
    cd build32
        ../configure --prefix=`pwd`/../install32 --disable-gdb --with-languages="c,c++" --with-arch=rv32im --with-abi=ilp32 --with-isa-spec="20191213"
        make -j`nproc`
    cd ../
cd ../


cd ../
