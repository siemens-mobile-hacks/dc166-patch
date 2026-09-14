# dc166

Patches and runtime libraries for TASKING C166/ST10 v8.6r1 Build 753.

Changes:

- removed compiler, linker and debugger demo restrictions;
- enabled `float`/`double` code generation, including conditional
  expressions, fixed their Classic ABI, IEEE add/subtract/multiply/divide,
  integer conversions and `float`/`double` width conversions, including
  signed zero, subnormal values, round-to-nearest-even, infinities and NaNs,
  and fixed volatile FP loads, stores and compound assignments in every
  memory model;
- added `c166` libraries for Tiny, Medium, Large and Huge;
- added patched `rt166` libraries for Small, Tiny, Medium, Large and Huge;
- added patched `fp166` libraries for Small, Tiny, Medium, Large and Huge;
- added C167 (`ext`) and Super10 (`ext2`) runtime variants;
- fixed CrossView ISS handling of byte writes to SFRs, CSP reads after
  `RETI`/`RETS`, PEC transfers at the end of `ATOMIC` windows, and branches
  ending at a code-segment boundary;
- fixed O3 Small/Tiny rejection of function pointers to `memcpy` and
  `memset`.

Required original installer:

```text
inputs/Tasking-C166-8.6r1-original.exe
```

## Build

Requires GNU make, GNU binutils, Perl, Wine, `unshield` and `7z`.

```sh
make
make verify
make rebuild
make clean
```

Generated files:

```text
patches/final/{c166,l166,xfw166,disim166}.vkp
lib/{ext,ext2}/c166{t,m,l,h}.lib
lib/{ext,ext2}/rt166{s,t,m,l,h}.lib
lib/{ext,ext2}/fp166{s,t,m,l,h}.lib
dist/Tasking-C166-8.6r1-patched.exe
```

## Tests

Requires Wine, Xvfb and TASKING installed in `WINEPREFIX` (default:
`~/.wine`). The input installer must also be present.

```sh
cmake -S . -B build
cmake --build build
tools/with-headless-x ctest --test-dir build --output-on-failure -j 1
```
