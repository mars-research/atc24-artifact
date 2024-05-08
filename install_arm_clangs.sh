#!/usr/bin/env bash

if [ ! -d "$PWD/clang_install_dir/arm_nacl" ]; then
cd llvm-SFI
git checkout arm_nacl
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_nacl" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_c" ]; then
cd llvm-SFI
git checkout arm_c
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_c" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_mte" ]; then
cd llvm-SFI
git checkout arm_mte
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_mte" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_pac" ]; then
cd llvm-SFI
git checkout arm_pac
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_pac" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_1reg" ]; then
cd llvm-SFI
git checkout arm_1reg
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_1reg" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_ldst" ]; then
cd llvm-SFI
git checkout arm_ldst
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_ldst" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/arm_retcall" ]; then
cd llvm-SFI
git checkout arm_retcall
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/arm_retcall" && cd build/ && cmake --build . --target install -j5 && exit"
cd ../
fi