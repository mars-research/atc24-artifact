#!/usr/bin/env bash
set -euo pipefail

NAME=$1
SRC=$2
DST=$3
CPU=$4
shift 4

if [[ -z "${OBJCOPY:-}" ]]; then
OBJCOPY=aarch64-unknown-linux-gnu-objcopy
fi

CFLAGS=()

case "$CPU" in
	aarch64)
		OBJ_TYPE="elf64-aarch64"
		BIN_ARCH="aarch64"
		;;
	x86_64)
		OBJ_TYPE="elf64-x86-64"
		BIN_ARCH="i386:x86-64"
		;;
	*)
		>&2 echo "$CPU not supported"
		exit 1
		;;
esac

set -x
TMP=$(mktemp -d --suffix=-dome-trampoline)
REAL_SRC=$(realpath "$SRC")
REAL_DST=$(realpath "$DST")

pushd "$TMP"

"$@" "${CFLAGS[@]}" -nostdlib -nostdinc -ffreestanding -o "trampoline.o" "$REAL_SRC"
"$OBJCOPY" -O binary -j .text "trampoline.o" "trampoline.bin"
"$OBJCOPY" -I binary -O "$OBJ_TYPE" --binary-architecture "$BIN_ARCH" \
--redefine-sym=_binary_trampoline_bin_start="_${NAME}_start" \
--redefine-sym=_binary_trampoline_bin_end="_${NAME}_end" \
--redefine-sym=_binary_trampoline_bin_size="_${NAME}_size" \
"trampoline.bin" "$REAL_DST"

popd

rm -rf "$TMP"
