# Runtime libraries

The build creates TASKING Classic `c166` and `rt166` libraries for Tiny (`t`),
Medium (`m`), Large (`l`) and Huge (`h`), and `fp166` libraries for Small
(`s`) through Huge, in both `ext` variants:

- `fp166?.lib`: floating-point arithmetic, conversions, comparisons and
  double load/store helpers.
- `rt166?.lib`: integer arithmetic, indirect-call, pointer and block-copy
  helpers.
- `c166?.lib`: libc, startup, allocators, strings, conversions, locale/time,
  narrow and wide stdio, formatted I/O and CrossView FSS calls.

Tiny and Medium use native NEAR entry points. Their adapters call the original
FAR helper bodies without changing the compiler ABI. Large and Huge use FAR
entry points. Model-specific libc modules are compiled with the matching
`-Mt`, `-Mm`, `-Ml` or `-Mh` option. Tiny omits explicit far/huge-only APIs;
Huge `malloc` may cross 16-KiB pages.

Model-independent scalar and explicitly qualified functions are imported from
the original archive. The far heap uses `?C166_FHEAP`; set its size with
`HEAPSIZE` or `-WlHEAPSIZE(...)`.

`runtime/manifests` records every imported, replaced and generated archive
member. `runtime/audit_libc.pl` and `runtime/audit_libc_symbols.pl` enforce
module classification and public-symbol parity with the original Small
library.
