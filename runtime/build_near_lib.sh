#!/usr/bin/env bash
set -euo pipefail

[[ $# -eq 7 ]] || {
  echo "usage: $0 INPUT.lib MANIFEST MODEL ASSEMBLER REGDEF WORK_DIR OUTPUT.lib" >&2
  exit 2
}

input="$(readlink -f -- "$1")"
manifest="$(readlink -f -- "$2")"
model="$3"
assembler="$(readlink -f -- "$4")"
regdef="$(readlink -f -- "$5")"
work_dir="$(readlink -m -- "$6")"
output="$(readlink -m -- "$7")"
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
wine_prefix="${WINEPREFIX:-$HOME/.wine}"

[[ "$model" == t || "$model" == m ]] || {
  echo "build_near_lib: model must be t or m" >&2
  exit 2
}

mkdir -p -- "$work_dir" "$(dirname -- "$output")"
cp -- "$regdef" "$work_dir/reg.def"
"$project_root/runtime/prepare_near_lib.pl" \
  "$input" "$manifest" "$model" "$work_dir"

wine_environment=(
  "WINEPREFIX=$wine_prefix"
  "WINEDEBUG=${WINEDEBUG:--all}"
  "WINEDLLOVERRIDES=${WINEDLLOVERRIDES:-winemenubuilder.exe=d;mscoree,mshtml=}"
)

for source in "$work_dir"/*-adapter.asm; do
  [[ -e "$source" ]] || break
  object="${source%.asm}.obj"
  if ! (
    cd "$work_dir"
    env "${wine_environment[@]}" wine "$assembler" \
      "$(basename -- "$source")" TO "$(basename -- "$object")" NOPR EXTEND \
      >tasking.log 2>&1
  ); then
    cat "$work_dir/tasking.log" >&2
    exit 1
  fi
done

"$project_root/runtime/pack_lib.pl" \
  "$work_dir" "$work_dir/order.txt" "$output"
