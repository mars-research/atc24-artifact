#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"
use_worktree spec-env
pushd "$worktree"

export SPEC="$worktree/specCPU2006"

if [[ ! -e "$SPEC/MANIFEST" ]]; then
	>&2 "$SPEC does not appear to be a valid SPEC install"
	exit 1
fi

>&2 echo "Applying patches..."
NIX_CFLAGS_COMPILE="-Wno-implicit-int" just setup-spec2006

>&2 echo "Copying configs..."
cp $base/spec2006-configs/* ./specCPU2006/config/
