#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree/specCPU2017"

tests=(intspeed fpspeed)
configs=(linux_x86_c linux_x86_nacl linux_x86_mpk linux_x86_gs)

run_test() {
	config="$1"
	SPEC=$PWD bin/runspec --config=$config --iterations=3 --threads=1 --copies=1 "${tests[@]}"
}

for config in "${configs[@]}"; do
	run_test "$config"
done
