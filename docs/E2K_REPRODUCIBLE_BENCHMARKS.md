# Reproducible E2K benchmark protocol

## Purpose and scope

This protocol adds a small, portable C11 user-space workload suite for MCST Elbrus Linux. It is intended to establish repeatable baselines and detect regressions for a fixed hardware/software configuration.

It is **not** a replacement for MCST's official benchmark suite, a full-system benchmark, a cache hierarchy characterization, or a hardware qualification report. It does not change firmware, microcode, clock settings, voltage, kernel configuration or privileged registers.

## Requirements

- Elbrus Linux on an E2K system for a native result.
- MCST LCC available as `lcc`, or set `E2K_CC` to the compiler executable.
- Python 3.
- `make`.

A user-mode QEMU run may be useful for functional smoke testing, but it must not be compared as a native performance result. Native mode is the default and requires `uname -m` to report `e2k`.

## Run

From the NUN repository root:

```sh
make e2k-host
make e2k-host-test
make e2k-bench
```

Optional workload parameters:

```sh
E2K_CC=/opt/mcst/lcc/bin/lcc \
NUN_E2K_BENCH_ITERATIONS=2000000 \
NUN_E2K_BENCH_REPEATS=7 \
NUN_E2K_BENCH_ELEMENTS=262144 \
make e2k-bench
```

The default suite runs five repetitions of:
1. `integer_mix`: deterministic integer dependency chain.
2. `sequential_memory_rw`: sequential read/modify/write over an allocated array.
3. `dependent_pointer_chase`: dependent loads through a deterministic single-cycle index array.

The workloads are deliberately portable C11. They do not assume undocumented E2K instructions or privileged access. The pointer-chase workload is a basic dependent-load exercise, not a complete cache-latency sweep.

## Output

The runner writes:

- `build/e2k/benchmark/results/microbench.csv`: raw measurements, one row per workload and repetition.
- `build/e2k/benchmark/results/metadata.json`: target identity, compiler version and flags, parameters, source SHA-256, commit and limitations.

CSV columns:

| Column | Meaning |
|---|---|
| `benchmark` | Workload identifier |
| `repeat` | Zero-based repetition |
| `elapsed_ns` | Monotonic-clock elapsed nanoseconds |
| `operations` | Workload-specific operation count |
| `checksum` | Result used to make computation observable and support consistency checks |

The benchmark workflow runs only when manually dispatched on a self-hosted GitHub Actions runner labelled `linux,e2k`. It uploads the executable, runtime manifest and benchmark evidence as a workflow artifact.

## Comparison rules

For a valid before/after comparison, keep constant:
- physical machine, CPU model/stepping and board revision;
- firmware/PNS, OS distribution, kernel and system configuration;
- MCST LCC version, sysroot, compiler flags and link options;
- workload parameters, source revision policy and execution procedure.

Record ambient system load and whether the machine was otherwise idle. Run the same test several times, retain every raw row, and report median, minimum, maximum and dispersion. Do not discard outliers without a written reason. A single run is not evidence of an optimization.

A performance claim should include the baseline commit and candidate commit, exact commands, raw CSVs, metadata, correctness checks and a reviewable explanation of the code change.

## Result classifications

- `native-e2k-tested`: executed on a host whose reported machine is `e2k`.
- `emulated`: executed only after explicit user-mode emulation opt-in.
- `cross-built`: compiled for E2K but not executed.
- `hardware-qualified`: **not** assigned automatically by this runner. It requires independent review and repeated evidence for the exact hardware/software configuration.

## Limitations and next extensions

This initial suite collects user-space elapsed time only. It does not collect energy, frequency, cache-miss counts, IPC, branch misses or other hardware counters. Add those only through documented, permissioned interfaces available on the target system, recording the exact counter source and semantics.

Next research iterations should add:
- working-set size sweeps and randomized pointer chains;
- separate integer, floating-point and vectorizable kernels;
- correctness/reference outputs for every workload;
- optional documented performance-counter collection;
- comparison tooling that checks metadata compatibility before calculating deltas;
- independent reproduction on a second E2K system.

Do not infer an E2K hardware optimization from an emulated run, an x86 host run, a successful compile, or a benchmark without a matched baseline.
