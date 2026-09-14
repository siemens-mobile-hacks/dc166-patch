# O3 rejects memory-function addresses in Small and Tiny models

## Summary

TASKING C166 v8.6r1 Build 753 rejects ordinary function-pointer initializers
for `memcpy` and `memset` when both of these conditions are true:

- optimization is `-O3`;
- memory model is Small (`-Ms`) or Tiny (`-Mt`).

The same declarations compile at O0, O1, O2, Os, and Oz in those models. They
also compile at O3 in Large, Medium, and Huge. An identically typed pointer to
`memmove` is accepted even in the failing configurations.

This is not a demo-size or license restriction. The source is nine
declarations, and the compiler emits semantic diagnostics followed by an
internal consistency failure. No demo-limit diagnostic is present.

## Impact

This blocks completion of the standalone `memory-builtins` differential test
in `/home/azq2/dev/sie/llvm-c166-tests`.

The test intentionally exercises two call paths for each runtime function:

1. a direct C call;
2. an indirect call through a volatile function pointer.

Both paths are ABI-relevant. Removing the pointer initializers, hiding the
function names from optimization, wrapping the functions, or omitting O3
would weaken the test and is therefore not an acceptable workaround.

The full matrix currently passes 146 of 150 CTest checks. The four reported
failures consist of two actual build failures and the two runtime tests that
CTest correctly does not start after their build fixtures fail:

```text
build.small.differential.memory-builtins.tasking.O3  failed
run.small.differential.memory-builtins.tasking.O3    not run
build.tiny.differential.memory-builtins.tasking.O3   failed
run.tiny.differential.memory-builtins.tasking.O3     not run
```

All 90 LLVM checks pass. The other 28 TASKING model/optimization builds and
runs pass with the same signature as LLVM.

## Minimal reproducer

Source:
[`tasking-o3-small-builtin-function-address-repro.c`](tasking-o3-small-builtin-function-address-repro.c)

```c
typedef unsigned int size_type;
typedef void *(*copy_function)(void *, const void *, size_type);
typedef void *(*set_function)(void *, int, size_type);

extern void *memcpy(void *, const void *, size_type);
extern void *memmove(void *, const void *, size_type);
extern void *memset(void *, int, size_type);

copy_function volatile copy_pointer = memcpy;
copy_function volatile move_pointer = memmove;
set_function volatile set_pointer = memset;
```

The `volatile` qualifier applies to the stored pointer object. It prevents the
test compiler from deleting or folding away the indirect-call path; it does
not change the pointed-to function type.

## Exact reproduction commands

The following commands use the compiler installed in the system Wine prefix.
They only invoke the C compiler front end and write intermediate assembly to
the external test scratch directory.

Failing Small/O3 configuration:

```sh
cd /tmp/llvm-c166-tests
WINEDEBUG=-all wine \
  "$HOME/.wine/drive_c/Program Files (x86)/TASKING/dc166 v8.6/bin/c166.exe" \
  'Z:\home\azq2\dev\ask\dc166\bugs\tasking-o3-small-builtin-function-address-repro.c' \
  -o small-o3.src -e -x -Hreg167.h -O3 -Ms
```

Failing Tiny/O3 configuration is identical except for `-Mt`:

```sh
cd /tmp/llvm-c166-tests
WINEDEBUG=-all wine \
  "$HOME/.wine/drive_c/Program Files (x86)/TASKING/dc166 v8.6/bin/c166.exe" \
  'Z:\home\azq2\dev\ask\dc166\bugs\tasking-o3-small-builtin-function-address-repro.c' \
  -o tiny-o3.src -e -x -Hreg167.h -O3 -Mt
```

Useful passing controls:

```sh
# Same Small model, lower optimization.
WINEDEBUG=-all wine "$HOME/.wine/drive_c/Program Files (x86)/TASKING/dc166 v8.6/bin/c166.exe" \
  'Z:\home\azq2\dev\ask\dc166\bugs\tasking-o3-small-builtin-function-address-repro.c' \
  -o small-o2.src -e -x -Hreg167.h -O2 -Ms

# Same O3 optimizer, Large model.
WINEDEBUG=-all wine "$HOME/.wine/drive_c/Program Files (x86)/TASKING/dc166 v8.6/bin/c166.exe" \
  'Z:\home\azq2\dev\ask\dc166\bugs\tasking-o3-small-builtin-function-address-repro.c' \
  -o large-o3.src -e -x -Hreg167.h -O3 -Ml
```

Observed process results:

| Configuration | Exit status | Result |
|---|---:|---|
| `-O3 -Ms` | 2 | E140/E160 for `memcpy` and `memset`, then S567 |
| `-O3 -Mt` | 2 | same failure |
| `-O2 -Ms` | 0 | accepted |
| `-O3 -Ml` | 0 | accepted |

## Actual diagnostics

Small/O3 reports:

```text
9: copy_function volatile copy_pointer = memcpy;
E 140: '&' cannot be applied to a register/bit/bitfield object or builtin/inline function
E 160: pointer mismatch at '='
       in initializer "copy_pointer"
11: set_function volatile set_pointer = memset;
E 140: '&' cannot be applied to a register/bit/bitfield object or builtin/inline function
E 160: pointer mismatch at '='
       in initializer "set_pointer"
S 567: internal consistency check failed - cannot continue
```

There is no diagnostic for this initializer in between them:

```c
copy_function volatile move_pointer = memmove;
```

This isolates the failure to special O3 handling of the recognized
`memcpy`/`memset` builtins rather than to the function-pointer type itself.

## Passing-code evidence

Small/O2 emits ordinary four-byte data pointers to all three FAR functions:

```asm
DSPTR _memcpy
_copy_pointer LABEL WORD
DS 4

DSPTR _memmove
_move_pointer LABEL WORD
DS 4

DSPTR _memset
_set_pointer LABEL WORD
DS 4

EXTERN _memmove:FAR
EXTERN _memset:FAR
EXTERN _memcpy:FAR
```

Large/O3 emits the same `DSPTR` initializers and FAR extern declarations.
Therefore the compiler already has the required relocation and ABI
representation; the failing path is specifically the Small/Tiny O3 builtin
classification or transformation.

## Full observed matrix

| Model | O0 | O1 | O2 | O3 | Os | Oz |
|---|---:|---:|---:|---:|---:|---:|
| Large | pass | pass | pass | pass | pass | pass |
| Medium | pass | pass | pass | pass | pass | pass |
| Small | pass | pass | pass | **fail** | pass | pass |
| Tiny | pass | pass | pass | **fail** | pass | pass |
| Huge | pass | pass | pass | pass | pass | pass |

## Likely failing compiler stage

The evidence points to an O3 builtin optimization performed before normal
function-designator conversion:

- only names recognized as memory builtins fail;
- an ordinary external function with the same pointer type (`memmove`) is
  accepted;
- changing only O3 to O2 makes Small/Tiny succeed;
- changing only the memory model from Small to Large makes O3 succeed;
- successful configurations emit the expected `DSPTR` relocation.

This identifies the boundary to inspect, but it is not proof of the precise
internal implementation error. The compiler source or a debugger trace is
needed to locate the exact faulty transform.

## Expected behavior

All three initializers must be accepted in every memory model and
optimization level. In Small and Tiny, the compiler must emit the same
four-byte function-pointer representation and compatible relocation that it
already emits at O2. Indirect calls through those pointers must reach the
runtime-library functions with the declared C calling convention.

The compiler must also never terminate with S567 for this valid input.

## Fix acceptance criteria

1. The minimal source compiles with exit status 0 for `-O3 -Ms` and
   `-O3 -Mt`.
2. Generated output contains relocatable initializers for `_memcpy`,
   `_memmove`, and `_memset`; none is silently replaced with zero or a wrapper
   of a different calling convention.
3. The complete `memory-builtins` matrix passes all 150 CTest checks:

   ```sh
   cd /home/azq2/dev/sie/llvm-c166-tests
   LLVM_BUILD=/home/azq2/dev/sie/llvm-work/build \
     ./run 'differential\.memory-builtins\.'
   ```

4. The Small/Tiny O3 runtime signatures equal `0x82415df9` (decimal
   `2185321977`), matching the independently calculated expected result and
   all other configurations.
