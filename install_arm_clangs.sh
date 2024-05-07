#!/usr/bin/env bash

if [ ! -d "$PWD/clang_install_dir/arm_nacl" ]; then
cd llvm-SFI
git checkout arm_nacl
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_nacl" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi
