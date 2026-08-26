#!/usr/bin/env bash
set -euo pipefail

[[ $# -eq 9 ]] || {
  echo "usage: $0 MODEL COMPILER ASSEMBLER REGDEF INCLUDE_DIR SOURCE_DIR MANIFEST WORK_DIR STAMP" >&2
  exit 2
}

model="$1"
compiler="$(readlink -f -- "$2")"
assembler="$(readlink -f -- "$3")"
regdef="$(readlink -f -- "$4")"
include_dir="$(readlink -f -- "$5")"
source_dir="$(readlink -f -- "$6")"
manifest="$(readlink -f -- "$7")"
work_dir="$(readlink -m -- "$8")"
stamp="$(readlink -m -- "$9")"
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
wine_prefix="${WINEPREFIX:-$HOME/.wine}"
tiny_excluded_manifest="$project_root/runtime/manifests/c166t-excluded.modules"

[[ "$model" == t || "$model" == m || "$model" == l || "$model" == h ]] || {
  echo "build_c166l_objects: invalid model: $model" >&2
  exit 2
}

mkdir -p -- "$work_dir"
cp -- "$regdef" "$work_dir/reg.def"
cp -- "$include_dir"/*.h "$work_dir/"
for header in "$source_dir"/*.h; do
  [[ -e "$header" ]] || break
  cp -- "$header" "$work_dir/"
done

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

tiny_excluded=()
while IFS= read -r member; do
  member="${member%%#*}"
  member="${member#"${member%%[![:space:]]*}"}"
  member="${member%"${member##*[![:space:]]}"}"
  [[ -n "$member" ]] && tiny_excluded+=("$member")
done < "$tiny_excluded_manifest"

while IFS= read -r member; do
  member="${member%%#*}"
  member="${member#"${member%%[![:space:]]*}"}"
  member="${member%"${member##*[![:space:]]}"}"
  [[ -n "$member" ]] || continue
  [[ "$member" == *.obj ]] || {
    echo "$manifest: invalid generated member: $member" >&2
    exit 1
  }
  stem="${member%.obj}"
  if [[ "$model" == t ]]; then
    for excluded in "${tiny_excluded[@]}"; do
      [[ "$member" == "$excluded" ]] && continue 2
    done
  fi
  c_source="$source_dir/$stem.c"
  asm_source="$source_dir/$stem.asm"
  [[ -f "$source_dir/${stem}_${model}.c" ]] && c_source="$source_dir/${stem}_${model}.c"
  [[ -f "$c_source" || -f "$asm_source" ]] || {
    echo "$manifest: missing source for $member" >&2
    exit 1
  }
  if [[ -f "$c_source" ]]; then
    cp -- "$c_source" "$work_dir/$stem.c"
    (
      cd "$work_dir"
      compiler_options=("-M$model" -O2 -AI -OX -I.)
      if [[ "$stem" == fss || "$stem" == simio ]]; then
        compiler_options+=(-g)
      fi
      run_wine "$compiler" "${compiler_options[@]}" -o "$stem.src" "$stem.c"
      run_wine "$assembler" "$stem.src" TO "$member" NOPR EXTEND
    )
  else
    "$project_root/runtime/render_model_asm.pl" "$model" \
      "$asm_source" "$work_dir/$stem.asm"
    (
      cd "$work_dir"
      run_wine "$assembler" "$stem.asm" TO "$member" NOPR EXTEND
    )
  fi
done < "$manifest"

touch -- "$stamp"
