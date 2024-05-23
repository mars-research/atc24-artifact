#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree"

export SPEC="$worktree/specCPU2017"

if [[ ! -e "$SPEC/MANIFEST" ]]; then
	>&2 "$SPEC does not appear to be a valid SPEC install"
	exit 1
fi

>&2 echo "Applying patches..."
patch -N -p1 < ../spec2017.patch

>&2 echo "Copying configs..."
cp $base/spec2017-configs/* ./specCPU2017/config/
