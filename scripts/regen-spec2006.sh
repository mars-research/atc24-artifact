#!/usr/bin/env bash
set -euo pipefail

TOP=$(realpath $(dirname $0)/..)

rundiff() {
	set +e
	diff "$@"
	ret=$?
	if [ $ret -gt 1 ]; then
		exit $ret
	fi
	set -e
}

pushd $TOP >/dev/null

if [ ! -e "specCPU2006" -o ! -e "specCPU2006.orig" ]; then
	echo 'You need to extract SPEC CPU2006 into `specCPU2006`, and an unmodified tree in `specCPU2006.orig`'
	exit 1
fi

>&2 echo "Regenerating patches..."

rundiff -Nur -x config -x build -x exe -x run -x result -x tmp -x output -x tools -x SUM.tools -x '*.rej' specCPU2006.orig/ specCPU2006/ | sed '/Binary\ files\ /d' | tee spec2006.patch
rundiff -Nur specCPU2006{.orig,}/tools/src/specinvoke/unix.c | tee -a spec2006.patch
