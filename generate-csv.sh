#!/usr/bin/env bash

BASE=$(dirname "$0")/

rm -f *.csv

function run_variant {
	>&2 echo "Running $1..."
	echo "depth, iterations, cycles, cycles/iterations, cycles/call"
	for i in $(seq 1 16); do
		>&2 echo -n "$i "
		$BASE/$1 $i 13
	done
	>&2 echo
}

function run_variant_one_domain {
	>&2 echo "Running $1..."
	echo "depth, iterations, cycles, cycles/iterations, cycles/call"
	for i in $(seq 1 16); do
		>&2 echo -n "$i "
		$BASE/$1 $i 1
	done
	>&2 echo
}

function run_variant_T_to_all_domain {
	>&2 echo "Running $1..."
	echo "depth, iterations, cycles, cycles/iterations, cycles/call"
	for i in $(seq 1 16); do
		>&2 echo -n "$i "
		$BASE/$1 $i 0
	done
	>&2 echo
}

run_variant ipc.full > ipc.full.csv
run_variant ipc.nowrpkru > ipc.nowrpkru.csv
run_variant ipc.base > ipc.base.csv
run_variant ipc.off > ipc.off.csv
run_variant ipc.regwipe > ipc.regwipe.csv
run_variant ipc.simd > ipc.simd.csv
run_variant ipc.nocheckrax > ipc.nocheckrax.csv
run_variant ipc.nocheckcaller > ipc.nocheckcaller.csv
