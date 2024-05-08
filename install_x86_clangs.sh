#!/usr/bin/env bash

if [ ! -d "$PWD/clang_install_dir/gs" ]; then
cd llvm-SFI
git checkout ColorGuard
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/gs" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/mpk" ]; then
cd llvm-SFI
git checkout MPK
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/mpk" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/nacl" ]; then
cd llvm-SFI
git checkout nacl_v2
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/nacl" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/reg" ]; then
cd llvm-SFI
git checkout nacl_v2_reg
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/reg" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/ldst" ]; then
cd llvm-SFI
git checkout nacl_v2_ldst
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/ldst" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/retcall" ]; then
cd llvm-SFI
git checkout nacl_v2_retcall
nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/retcall" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi

if [ ! -d "$PWD/clang_install_dir/c" ]; then
cd llvm-SFI
git checkout nacl_v2_c
nix develop --command bash -c -v "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX="$PWD/../clang_install_dir/c" && cd build/ && cmake --build . --target install -j128 && exit"
cd ../
fi