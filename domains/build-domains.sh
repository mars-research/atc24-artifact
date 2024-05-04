#!/usr/bin/env bash
set -euo pipefail

dir=$(realpath $(dirname $0))
domain_type="$1"
machine=$(uname -m)

declare -a CFLAGS

case "$machine" in
	x86_64)
		CC=clang
		CFLAGS+=("-masm=intel")
		;;
	aarch64)
		CC=aarch64-unknown-linux-gnu-clang
		CFLAGS+=("-Wl,--image-base=0x10000")
		;;
	*)
		>&2 echo "Unsupported machine $machine"
		exit 1
		;;
esac

CFLAGS+=(
	"-O2"

	"-I$dir/../$domain_type/runtime"

	"-Wl,--defsym=_${domain_type}_call_slots=0x0"

	"-static-pie"
	"-fPIC"
	"-ffreestanding"
	"-nostdlib"
	"-fno-plt"
	"-pedantic"
	"-Wno-language-extension-token"
)

>&2 echo "[!!] Building for $domain_type"

pushd "$dir"
set -x
#"$CC" "${CFLAGS[@]}" foo.c -e foo -o foo.so
#"$CC" "${CFLAGS[@]}" bar.c -e bar -o bar.so
#"$CC" "${CFLAGS[@]}" baz.c -e baz -o baz.so

"$CC" "${CFLAGS[@]}" pong.c -e main -o pong-main.so
"$CC" "${CFLAGS[@]}" pong.c -e pong -o pong.so
popd
