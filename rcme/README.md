# RCME native module

Repository-specific native C11 adapter for the Recursive Cross-Domain Matrix Engine.

## Scope
- Caller-owned matrix views; no heap allocation, global mutable state, or implicit I/O.
- Checked matrix multiplication and transpose.
- One bounded recursive composition: normalize(M × A × transpose(M) + B).
- Frobenius normalization, residual measurement, finite-value and dimension checks.
- Domain-specific diagonal weighting for NUN: Kernel memory, execution, and architecture contract evidence.

## Build and test
```sh
cc -std=c11 -Wall -Wextra -Werror -pedantic -Ircme/include \
  rcme/src/rcme.c rcme/src/rcme_domain.c rcme/tests/test_rcme.c -lm -o rcme-test
./rcme-test
```

This is an isolated additive slice, not a kernel, bootloader, hardware controller, or quantum-computing implementation. It does not modify existing application APIs or infer physical laws. External domain data requires separately validated adapters.
