#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
configs=(arm_nacl arm_c arm_mte arm_pac arm_1reg arm_ldst arm_retcall)

for config in "${configs[@]}"; do
	>&2 echo "Building $config"
	cd "$base/llvm-SFI"
	git checkout "$config"
	nix develop --command bash -c  "cmake \$cmakeFlags -Sllvm -Bbuild -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=$base/clang_install_dir/$config && cd build/ && cmake --build . --target install -j5 && exit"
done
