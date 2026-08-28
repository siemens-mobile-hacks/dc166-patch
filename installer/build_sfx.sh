#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "$0")" && pwd)
project_dir=$(cd -- "$script_dir/.." && pwd)
tasking_installer=${TASKING_C166_INSTALLER:-$project_dir/inputs/Tasking-C166-8.6r1-original.exe}
c166_patched=${C166_PATCHED:-$project_dir/build/work/c166-patched.exe}
l166_patched=${L166_PATCHED:-$project_dir/build/work/l166-patched.exe}
xfw166_patched=${XFW166_PATCHED:-$project_dir/build/work/xfw166-patched.exe}
runtime_lib_root=${RUNTIME_LIB_ROOT:-$project_dir/lib}
temp_parent=/tmp/codex/dc166-installer
sfx_module=$script_dir/vendor/7zSD.sfx
output=$project_dir/dist/Tasking-C166-8.6r1-patched.exe

test -f "$tasking_installer"
test -f "$sfx_module"
test -f "$c166_patched"
test -f "$l166_patched"
test -f "$xfw166_patched"
for variant in ext ext2; do
    for model in t m l h; do
        for family in c166 fp166 rt166; do
            test -f "$runtime_lib_root/$variant/$family$model.lib"
        done
    done
done

mkdir -p "$temp_parent" "$project_dir/dist"
build_dir=$(mktemp -d "$temp_parent/build.XXXXXX")
trap 'rm -rf -- "$build_dir"' EXIT

mkdir -p "$build_dir/package/payload/lib/ext" \
    "$build_dir/package/payload/lib/ext2"

cp "$script_dir/setup.cmd" "$build_dir/package/setup.cmd"
cp "$tasking_installer" "$build_dir/package/original-installer.exe"
cp "$c166_patched" "$build_dir/package/payload/c166-patched.exe"
cp "$l166_patched" "$build_dir/package/payload/l166-patched.exe"
cp "$xfw166_patched" "$build_dir/package/payload/xfw166-patched.exe"
for variant in ext ext2; do
    cp "$runtime_lib_root/$variant/"*.lib \
        "$build_dir/package/payload/lib/$variant/"
done

(
    cd "$build_dir/package"
    7z a -t7z -mx=7 -ms=off -mtm=off -mta=off -mtc=off \
        "$build_dir/payload.7z" setup.cmd original-installer.exe payload >/dev/null
)

cp "$sfx_module" "$build_dir/installer.exe"
dd if="$script_dir/sfx-config.txt" of="$build_dir/installer.exe" bs=64K oflag=append conv=notrunc status=none
dd if="$build_dir/payload.7z" of="$build_dir/installer.exe" bs=1M oflag=append conv=notrunc status=none
cp "$build_dir/installer.exe" "$output"
