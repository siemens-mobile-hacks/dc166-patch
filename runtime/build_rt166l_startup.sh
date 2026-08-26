#!/usr/bin/env bash
set -euo pipefail

[[ $# -eq 8 ]] || {
  echo "usage: $0 MODEL PREPROCESSOR ASSEMBLER SOURCE INCLUDE_DIR WORK_DIR OBJECT STAMP" >&2
  exit 2
}

model="$1"
preprocessor="$(readlink -f -- "$2")"
assembler="$(readlink -f -- "$3")"
source="$(readlink -f -- "$4")"
include_dir="$(readlink -f -- "$5")"
work_dir="$(readlink -m -- "$6")"
object="$(readlink -m -- "$7")"
stamp="$(readlink -m -- "$8")"
wine_prefix="${WINEPREFIX:-$HOME/.wine}"
case "$model" in
  t) model_name=TINY ;;
  m) model_name=MEDIUM ;;
  l) model_name=LARGE ;;
  h) model_name=HUGE ;;
  *) echo "build_rt166l_startup: invalid model: $model" >&2; exit 2 ;;
esac

mkdir -p -- "$work_dir"
cp -- "$include_dir"/* "$work_dir/"
cp -- "$source" "$work_dir/cstart.asm"

wine_environment=(
  "WINEPREFIX=$wine_prefix"
  "WINEDEBUG=${WINEDEBUG:--all}"
  "WINEDLLOVERRIDES=${WINEDLLOVERRIDES:-winemenubuilder.exe=d;mscoree,mshtml=}"
)

run_wine() {
  local log="$work_dir/tasking.log"
  if ! env "${wine_environment[@]}" wine "$@" >"$log" 2>&1; then
    cat "$log" >&2
    return 1
  fi
}

(
  cd "$work_dir"
  run_wine "$preprocessor" cstart.asm "DEF(MODEL,$model_name)" TO cstart-model.asm
  assembler_architecture=EXTEND
  [[ "$(basename -- "$source")" == cstartx2.asm ]] && assembler_architecture=EXTEND2
  run_wine "$assembler" cstart-model.asm TO cstart.obj NOPR "$assembler_architecture"
)

if [[ "$work_dir/cstart.obj" != "$object" ]]; then
  cp -- "$work_dir/cstart.obj" "$object"
fi
touch -- "$stamp"
