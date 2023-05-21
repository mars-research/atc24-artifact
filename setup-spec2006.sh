#!/usr/bin/env bash
set -euo pipefail

TOP=$(realpath $(dirname $0))
SPEC="$TOP/specCPU2006"

fwdpatch() {
	# https://stackoverflow.com/a/39038298
	if patch -p1 --dry-run --reverse --force < "$1" >/dev/null 2>&1; then
		echo "Patch already applied - skipping."
	else # patch not yet applied
		echo "Patching..."
		patch -Ns -p1 < "$1" || echo "Patch failed" >&2 && exit 1
	fi
}

if [ ! -e "$SPEC/MANIFEST" ]; then
	echo 'You need to extract SPEC CPU2006 into `specCPU2006`'
	exit 1
fi

echo "Creating dummy tool checksum..."
echo >$SPEC/SUMS.tools

echo "Applying patches..."
pushd $SPEC >/dev/null
fwdpatch "$TOP/spec2006.patch"
popd >/dev/null

echo "Building specinvoke..."
pushd $SPEC/tools/src/specinvoke >/dev/null
./configure --prefix=$SPEC
make install
popd >/dev/null

echo "Linking specmake..."
pushd $SPEC >/dev/null
ln -sf $(which make) "$SPEC/bin/specmake"
popd >/dev/null
