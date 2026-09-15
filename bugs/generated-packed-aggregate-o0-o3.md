# Generated packed aggregates fail at O0 and O3

## Summary

The installed TASKING C166 compiler miscompiles a generated aggregate corpus
in all four tested memory models (Large, Medium, Small, and Huge):

- at O0, execution does not return from `generated_run()`;
- at O3, two packed five-byte aggregate cases return deterministic wrong
  values, and some models also show adjacent stack/pointer corruption.

O1, O2, Os, and Oz pass. The same source passes with LLVM at every
optimization level. This is not a compiler demo/size restriction and not a
CrossView transport failure: the O0 image starts, emits the TAP header and
plan, and then remains inside the tested function.

## Source

The complete reproducer is the shared-source test in:

```text
/home/azq2/dev/sie/llvm-c166-tests/tests/abi/abi-generated-aggregate/test.c
/home/azq2/dev/sie/llvm-c166-tests/tests/abi/abi-generated-aggregate/functions.c
/home/azq2/dev/sie/llvm-c166-tests/tests/abi/abi-generated-aggregate/corpus.inc
/home/azq2/dev/sie/llvm-c166-tests/tests/abi/abi-generated-aggregate/vectors.inc
```

Both TASKING and LLVM compile exactly `test.c` and `functions.c`. Compiler
spelling differences for packed records are confined to `corpus.inc`.

Configure and run only this matrix with:

```sh
cd /home/azq2/dev/sie/llvm-c166-tests
LLVM_BUILD=/home/azq2/dev/sie/llvm-work/build \
  ./run 'abi\.generated-aggregate'
```

One failing O3 configuration can be reproduced with:

```sh
LLVM_BUILD=/home/azq2/dev/sie/llvm-work/build \
  ./run '^(build|run)\.large\.abi\.generated-aggregate\.tasking\.O3$'
```

One failing O0 configuration can be reproduced with:

```sh
LLVM_BUILD=/home/azq2/dev/sie/llvm-work/build \
  ./run '^(build|run)\.large\.abi\.generated-aggregate\.tasking\.O0$'
```

The CMake command for Large/O3 is equivalent to:

```text
cc166.exe -Ml -x EXTEND -Hreg167.h SN(reg167.def)
  -WoSECSIZE(C166_US(+0880H)) -O3
  -DC166_TEST_TASKING=1 -DC166_TEST_SMALL=0
  -I.../support/include -g -ieee -v -o test.abs
  support/asc0.c support/tap.c test.c functions.c
```

Removing `-g` does not change the O3 failures.

## Result matrix

| Optimization | LLVM L/M/S/H | TASKING L/M/S/H |
|---|---|---|
| O0 | pass | hangs after TAP plan |
| O1 | pass | pass |
| O2 | pass | pass |
| O3 | pass | shapes 18 and 28 fail |
| Os | pass | pass |
| Oz | pass | pass |

The full CTest run produced 112 passes and eight failures. The eight failures
are exactly TASKING O0 and O3 in the four memory models.

## O3 wrong results

The seed is `0x5a5a`. Every model produces the same values:

```text
shape 18: expected 0x6f9c, actual 0x19db
shape 28: expected 0x16c2, actual 0x155f
```

Both failing shapes are packed five-byte records passed after a scalar
argument.

Shape 18:

```c
_packed struct gen_shape_18 {
  unsigned int f0;
  unsigned char f1;
  unsigned int f2;
};

static unsigned int gen_consume_18(
    unsigned int tail, _packed struct gen_shape_18 value);

output[17] = gen_consume_18(
    (unsigned int)(seed ^ 0x929aU), gen_make_18(seed));
```

Shape 28 contains a nested packed record:

```c
_packed struct gen_pinner {
  unsigned char byte;
  unsigned int word;
};

_packed struct gen_shape_28 {
  unsigned char f0;
  _packed struct gen_pinner f1;
  unsigned char f2;
};

static unsigned int gen_consume_28(
    unsigned int tail, _packed struct gen_shape_28 value);

output[27] = gen_consume_28(
    (unsigned int)(seed ^ 0x940cU), gen_make_28(seed));
```

On Large and Huge, the O3 run also emits corrupted bytes adjacent to the TAP
test-name string. Medium and Small retain the string but return the same two
wrong values. This is consistent with an aggregate result copy targeting the
wrong user-stack location.

## O3 assembly evidence

Direct `c166.exe -e` output shows a concrete O2/O3 difference in both
`gen_make_18` and `gen_make_28`. The helper call is `__cpnnb`, which copies the
five-byte packed return object to caller-reserved storage.

Passing O2 prepares the copy destination in R10:

```asm
MOV     R4,R0
MOV     R10,#06h
ADD     R10,R0
MOV     R3,#05h
CALLS   SEG __cpnnb,__cpnnb
```

Failing O3 instead prepares R12:

```asm
MOV     R4,R0
MOV     R12,#06h
ADD     R12,R0
MOV     R3,#05h
CALLS   SEG __cpnnb,__cpnnb
```

The call sites themselves are otherwise identical: both reserve five bytes,
call `gen_make_N`, load the scalar tail into R12, call `gen_consume_N`, and
then release five bytes. The wrong fixed register at the `__cpnnb` boundary is
therefore the leading O3 root-cause candidate.

Generated assembly used for this comparison is available during diagnosis at:

```text
/tmp/llvm-c166-tests/generated-aggregate-o2.src
/tmp/llvm-c166-tests/generated-aggregate-o3.src
```

## O0 nontermination

At O0 every model emits:

```text
TAP version 13
1..32
```

and no individual test line. The timeout was restored to the original legacy
value of 180 seconds; Large/O0 still expired after 181.57 seconds. Thus this is
not the temporary 90-second timeout introduced during migration.

The O0 assembly is much larger (`functions.obj` is about 70 KiB), but the
corpus contains only bounded loops over records of at most eight bytes. No C
loop has an unbounded exit condition. The exact O0 control-flow fault still
needs to be located after the O3 aggregate-copy bug is fixed, because the same
packed-return path is exercised before `generated_run()` returns.

## Controls

- LLVM passes all 32 values in all four models at O0/O1/O2/O3/Os/Oz.
- TASKING passes the identical C source at O1/O2/Os/Oz in all four models.
- TASKING O3 fails identically with and without debug information.
- The expected values come from the existing generated vectors and were also
  used by the previous mixed-image test.
- The simulator emits the TAP plan before O0 stalls, proving that startup,
  image loading, ASC0, and TAP transport are operational.

## Expected fix

1. Preserve the runtime helper's required aggregate-copy destination register
   at O3; do not allocate that operand to R12.
2. Verify packed aggregate returns of odd sizes, especially five bytes, when
   immediately forwarded as a later argument after a scalar.
3. Re-run the complete shared-source matrix above.
4. If O0 still stalls after the copy fix, trace the first packed shape that
   fails to return and fix that control-flow/stack-cleanup path separately.
