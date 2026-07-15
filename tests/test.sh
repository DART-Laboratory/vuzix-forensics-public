#!/usr/bin/env bash

set -euo pipefail

hs=../build/hs

for dir in ../logs/*/; do
	[[ -d "$dir" ]] || continue
	out_dir="$(basename "$dir")"

	echo
	echo "==== $out_dir TEST ===="
	echo

	bugreport=$(find "$dir" -maxdepth 1 -type f -name 'bugreport-*' -print -quit)

	[[ -n "$bugreport" ]] || continue

	rm -r "$out_dir"
	mkdir "$out_dir"

	$hs -d -o "$out_dir/graph" "$bugreport"
done
