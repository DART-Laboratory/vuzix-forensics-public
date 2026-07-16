#!/usr/bin/env bash

set -euo pipefail

hs=../build/hs

for dir in ../logs/*/; do
	[[ -d "$dir" ]] || continue
	out_dir="$(basename "$dir")"

	bugreport=$(find "$dir" -maxdepth 1 -type f -name 'bugreport-*' -print -quit)

	[[ -n "$bugreport" ]] || continue

	rm -r "$out_dir"
	mkdir "$out_dir"

	$hs -d -s -o "$out_dir/graph" "$bugreport" > "$out_dir/hs_out.txt"

	echo "$out_dir test finished"
done
