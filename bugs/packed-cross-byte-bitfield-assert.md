# Packed crossing-byte bit-field assignment triggers S 723

## Summary

The C166 frontend aborts while compiling an ordinary assignment to a 16-bit
bit-field that starts one bit into a packed object. The failure occurs in all
five memory models at every optimization level except O0.

This blocks removal of the hand-written assembler workaround from the
`abi-aggregate-layout` LLVM/TASKING differential test. The test intentionally
uses the normal C field assignment and must not reconstruct the object through
raw bytes.

## Reproducer

`packed-cross-byte-bitfield-assert.c` in this directory:

```c
typedef unsigned int u16;

_packed struct packed_bits {
  unsigned int first : 1;
  unsigned int second : 16;
};

u16 assign_packed_bits(u16 seed) {
  _packed struct packed_bits value;

  value.first = (seed >> 15) & 1U;
  value.second = seed ^ 0xbeefU;
  return value.second;
}
```

## Command

Run from a writable directory under Wine:

```sh
wine "C:\\Program Files (x86)\\TASKING\\dc166 v8.6\\bin\\cc166.exe" \
  -Ml -O2 -c -o repro.obj \
  'Z:\\home\\azq2\\dev\\ask\\dc166\\bugs\\packed-cross-byte-bitfield-assert.c'
```

## Actual result

The compiler exits with status 1 and prints:

```text
Z:\home\azq2\dev\ask\dc166\bugs\packed-cross-byte-bitfield-assert.c:
    14: }
S 723: (line 14) assertion failed - please report
```

In the full aggregate-layout source the same failure is sometimes preceded by:

```text
W 195: constant expression out of range -- truncated
S 723: assertion failed - please report
```

## Configuration matrix

| Model | O0 | O1 | O2 | O3 | Os | Oz |
|---|---:|---:|---:|---:|---:|---:|
| Large | pass | fail | fail | fail | fail | fail |
| Medium | pass | fail | fail | fail | fail | fail |
| Small | pass | fail | fail | fail | fail | fail |
| Tiny | pass | fail | fail | fail | fail | fail |
| Huge | pass | fail | fail | fail | fail | fail |

## Expected result

All configurations compile successfully. The assignment stores the low 16
bits of `seed ^ 0xbeefU` into `second`, preserving the adjacent one-bit field
and the packed three-byte object representation.
