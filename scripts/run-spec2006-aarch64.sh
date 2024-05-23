#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree/specCPU2006"

tests=(401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar)
configs=(linux_arm_c linux_arm_pac linux_arm_nacl linux_arm_reg linux_arm_mte linux_arm_retcall)

run_test() {
	config="$1"
	SPEC=$PWD bin/runspec --config=$config --iterations=3 --rebuild "${tests[@]}"
}

for config in "${configs[@]}"; do
	run_test "$config"
done
