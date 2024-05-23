#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree/specCPU2006"

tests=(401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp)
configs=(linux_x86_c linux_x86_reg linux_x86_ldst linux_x86_retcall linux_x86_nacl linux_x86_gs linux_x86_mpk)

run_test() {
	config="$1"
	SPEC=$PWD bin/runspec --config=$config --iterations 1 "${tests[@]}"
}

for config in "${configs[@]}"; do
	run_test "$config"
done
