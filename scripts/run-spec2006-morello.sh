#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree/specCPU2006"

tests=(all)
configs=(morello_specCPU2006)

run_test() {
	config="$1"
	SPEC=$PWD bin/runspec --config=$config --iterations=3 --rebuild "${tests[@]}"
}

for config in "${configs[@]}"; do
	run_test "$config"
done
