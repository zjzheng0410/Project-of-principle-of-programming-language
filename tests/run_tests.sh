#!/usr/bin/env bash
set -euo pipefail

cc=${CC:-gcc}
cflags=${CFLAGS:-"-std=c11 -Wall -Wextra -pedantic -g"}

mkdir -p build generated

fixtures=(
  option_match
  option_chain
  functional_closure
  arrays
  recursion_guard
)

for name in "${fixtures[@]}"; do
  src="tests/fixtures/${name}.ppl"
  gen="generated/test_${name}.c"
  bin="build/test_${name}.bin"
  out="build/test_${name}.out"
  golden="tests/golden/${name}.out"

  ./build/pplc "$src" -o "$gen"
  "$cc" -Iruntime $cflags -o "$bin" "$gen" runtime/*.c
  "$bin" > "$out"
  diff -u "$golden" "$out"
  printf 'PASS %s\n' "$name"
done

name=2026_feature_demo
src="examples/2026_feature_demo.ppl"
gen="generated/test_${name}.c"
bin="build/test_${name}.bin"
out="build/test_${name}.out"
golden="tests/golden/${name}.out"

./build/pplc "$src" -o "$gen"
"$cc" -Iruntime $cflags -o "$bin" "$gen" runtime/*.c
"$bin" > "$out"
diff -u "$golden" "$out"
printf 'PASS %s\n' "$name"
