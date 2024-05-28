#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"

use_worktree dome
pushd "$worktree"

clean() {
	rm -rf build
	meson setup -Dbuildtype=release ./build
}

build() {
	engine="$1"
	meson configure -Ddefault-engine="$engine" ./build
	ninja -C build
	./domains/build-domains.sh "$engine"
}

run() {
	engine="$1"
	./build/pong >"$results/x86-64-${engine}.csv"
}

set -x
clean
for engine in null ideal; do
	build "$engine"
	run "$engine"
done
