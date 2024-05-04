#!/usr/bin/env bash
set -euo pipefail
name="dome"

if [[ "$#" != 1 ]]; then
	>&2 echo "Usage: $0 <path to $name>"
fi

base="$(readlink -f $(dirname "$0"))"
repo="$1"

>&2 echo "  Base: $base"
>&2 echo "  Repo: $repo"

commit_human="$(git -C "$repo" describe --always --long HEAD)"
branch="${name}-${commit_human}"
>&2 echo "Commit: $commit_human"
>&2 echo "Branch: $branch"
>&2 echo

commit="$(git -C "$repo" bundle create - HEAD | git bundle unbundle - HEAD | cut -d' ' -f1)"
git branch --force "$branch" "$commit"

echo "$branch"
