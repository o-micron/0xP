mkdir thirdparty
cd thirdparty

git clone --depth 1 --single-branch -b v2025.10.5 https://github.com/shader-slang/slang.git
cd slang
git submodule update --init --recursive
cmake -S . -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/
cmake --build ./build/ --config Release --target install --parallel
cd ..

cd ..