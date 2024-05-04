# Use as:
#
# source "$(dirname "${BASH_SOURCE[0]}")/prelude.sh"

set -euo pipefail
base="$(readlink -f $(dirname "${BASH_SOURCE[0]}")/..)"
results="$base/results"

mkdir -p "$results"

get_worktree_path() {
	name="$1"
	path="$base/worktrees/$name"

	if [[ -d "$path" ]]; then
		>&2 echo "Found $name: $path"
		echo "$path"
		exit 0
	fi

	branch=$(get_worktree_branch "$name")
	if [[ -z "${branch}" ]]; then
		>&2 echo "No such project: $name"
		exit 1
	fi

	git worktree add -d "$path" "$branch"
}

get_worktree_branch() {
	name="$1"
	branch="$(grep "$name" "$base/branches" | cut -d' ' -f2)"
	echo "$branch"
}

use_worktree() {
	name="$1"
	worktree="$(get_worktree_path "$name")"

	env="$(nix print-dev-env "$worktree")"
	eval "$env"
}
