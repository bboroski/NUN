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
