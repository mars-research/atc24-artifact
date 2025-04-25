#!/usr/bin/env bash

BASE=$(dirname "$0")/

function run_mte_test {
	>&2 echo "Running $1..."
	for i in $(seq 4 21); do
		>&2 echo -n "$i "
		 taskset -c 8 $BASE/$1 $((2 ** $i))
	done
	>&2 echo
}

run_mte_test mte_test > mte_test.csv