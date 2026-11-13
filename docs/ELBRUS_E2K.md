# NUN on Elbrus (E2K)

## Supported deployment route in this phase

`MCST PNS -> Elbrus Linux -> NUN-E2K-host`

NUN now has a native Linux userspace bootstrap runtime built by MCST LCC. It is
an ELF64 little-endian E2K executable with `e_machine=175`. The build script
generates a companion SHA-256 manifest and rejects a non-E2K ELF output.

On a supported Elbrus Linux system:

    make e2k-host
    make e2k-host-test

Set `E2K_CC` if the vendor compiler is not named `lcc`. For a cross build on
x86_64, use the MCST cross LCC package and target sysroot; compilation and ELF
validation do not prove the binary will run on the host.

The native E2K workflow is manually dispatched on a self-hosted runner labelled
`linux,e2k`. It executes the runtime, not merely compiles it.

## Important kernel boundary

The existing Limine kernel is x86_64-only. This E2K work adds a native E2K
Linux runtime process; it does **not** claim that the freestanding x86 kernel,
its Limine entry point, its COM1 port I/O or its framebuffer backend work on
E2K.

A standalone NUN kernel needs a target-specific PNS/kernel image contract and
reviewed E2K entry, privilege, memory-map, interrupt, exception and device
initialization code. Publicly available PNS information establishes its board
initialization and OS-loading role, but is not a complete public ABI for loading
an arbitrary third-party kernel image. Do not flash or overwrite firmware.

## Verification

The runtime manifest validates ELF64/LE, `EM_MCST_ELBRUS=175`, entry point,
file size and SHA-256. SHA-256 does not authenticate the publisher. Secure Boot
and a signed publisher trust chain are not implemented.


## Integrated native C11 qualification (added 2026-11-13)

The NUN workspace already composes the portable C11 components from Kloz,
CO, SaintSI, ReaperCripple and ToolNG PolicyCore. The E2K build now also
compiles the existing Kloz and CO architecture probes into their corresponding
static libraries. On an E2K build, CTest includes
`nun_e2k_interop_tests`, which requires the independently implemented
Kloz, CO, ToolNG and ReaperCripple probes to agree on:

- the `__e2k__` compiler target;
- 64-bit pointers and little-endian execution;
- ELF machine `EM_MCST_ELBRUS = 175`.

Run the complete native qualification sequence on an actual E2K Linux host:

```sh
make e2k-ecosystem-test
```

This command rejects non-E2K hosts, configures the root NUN CMake project with
the MCST LCC toolchain, builds the integrated C11 targets and runs the complete
CTest suite. It writes
`build/e2k-ecosystem/e2k-ecosystem-qualification.txt` only after all tests
pass. The report records host identity, compiler version and source commit.
The GitHub Actions workflow is manually dispatched and requires a self-hosted
runner labelled `linux,e2k`.

The interop test is a consistency/ABI-target check, not proof of optimized
instruction scheduling, cache behavior, performance gain, bare-metal boot,
or full hardware qualification of every attached device. Performance claims
still require workload-specific baselines, repeated measurements and recorded
compiler/CPU/firmware details.
