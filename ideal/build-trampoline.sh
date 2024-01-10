#!/usr/bin/env bash
set -euo pipefail

NAME=$1
SRC=$2
DST=$3
shift 3

OBJCOPY=aarch64-unknown-linux-gnu-objcopy

set -x
"$@" -nostdlib -nostdinc -ffreestanding -o - $SRC \
| "$OBJCOPY" -O binary -j .text - - \
| "$OBJCOPY" -I binary -O elf64-aarch64 --binary-architecture aarch64 \
--redefine-sym=_binary__stdin__start="_${NAME}_start" \
--redefine-sym=_binary__stdin__end="_${NAME}_end" \
--redefine-sym=_binary__stdin__size="_${NAME}_size" \
- $DST
