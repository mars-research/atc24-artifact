#!/usr/bin/env bash
set -euo pipefail

CFLAGS="-static-pie -fPIC -ffreestanding -fno-plt -nostdlib -Wl,--defsym=_morello_call_slots=0x0 -Wl,--image-base=0x10000 -pedantic -Wno-language-extension-token"

aarch64-unknown-linux-gnu-clang $CFLAGS foo.c -e foo -o foo.so
aarch64-unknown-linux-gnu-clang $CFLAGS bar.c -e bar -o bar.so
aarch64-unknown-linux-gnu-clang $CFLAGS baz.c -e baz -o baz.so
