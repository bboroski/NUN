# Fortran, Pascal and Delphi as NUN System-Language Profiles

**Status:** language integration architecture; no compiler front end or runtime is integrated into NUN yet.

## 1. One system ABI, multiple language ecosystems

NUN will expose a stable, versioned C ABI for syscalls and user-space services. Fortran, Pascal/Free Pascal and Delphi are supported through compiler front ends, runtime profiles and binding libraries. They do not become kernel implementation dependencies.

The operating-system kernel remains freestanding and uses a restricted C11 foundation plus architecture-specific assembly. The kernel must not assume that a Fortran runtime, Pascal RTL, Delphi package loader, garbage collector, GUI component framework or exception runtime is present during boot.

Language support is separated into four independent layers:

1. **Compiler/toolchain:** source parser, type checking, object generation, linker and debug information.
2. **Language runtime:** memory, strings, exceptions, I/O, initialization/finalization and threading.
3. **NUN binding library:** syscall wrappers and typed service interfaces over the stable C ABI.
4. **Developer experience:** formatter, diagnostics, debugger integration, examples, package metadata and conformance tests.

## 2. Fortran profile — numerical and scientific systems

Preserve the language capabilities that matter to engineering workloads:

- modules, submodules, explicit interfaces and kind-selected numeric types;
- derived types, allocatable and pointer components, array sections and assumed-shape arrays;
- column-major storage semantics and explicit array descriptor rules at language boundaries;
- intrinsic numeric, IEEE arithmetic and environment facilities;
- `ISO_C_BINDING` for stable interoperability with the NUN C ABI;
- coarrays and parallel language features only when a real runtime transport and scheduler backend exist;
- OpenMP or vendor extensions only through optional, target-qualified runtime providers.

**Initial support path:** external Fortran compilers produce user-space objects that link to a NUN Fortran binding library. Use `bind(C)` interfaces and interoperable types for syscalls; do not expose compiler-private descriptors or module symbol names as a stable kernel ABI.

The Fortran runtime may require compiler-specific startup, I/O, allocation, math and coarray services. These dependencies must be packaged as user-space runtime components and declared in the package manifest. A freestanding or no-I/O profile is a separate deliverable, not an automatic property of compiling Fortran.

## 3. Pascal / Free Pascal profile — typed systems and application code

Preserve and map:

- units and dotted-unit namespaces;
- ordinal/enumerated types, sets, records, variant records and packed data;
- pointers and procedural types behind explicit unsafe/FFI boundaries;
- classes, interfaces, properties and deterministic `try..finally` resource cleanup;
- generics, operator overloads and helper types when supported by the selected compiler;
- exceptions and RTTI as runtime services rather than kernel primitives;
- object initialization/finalization and managed strings/dynamic arrays through the Pascal RTL profile.

**Initial support path:** Free Pascal is the open compiler candidate for early cross-compilation experiments. Its target, ABI, RTL subset and exception model must be tested per architecture. Provide `Nun.Sys`, `Nun.Memory`, `Nun.Process`, `Nun.Storage` and `Nun.Device` units as wrappers over C ABI service calls.

A minimal runtime profile must explicitly state which managed types, exceptions, file I/O, thread services and RTTI features are available. No feature is advertised as freestanding until startup objects, allocator hooks, exception unwinding and linking pass target tests.

## 4. Delphi / Object Pascal profile — component model and rich applications

Expose a compatibility target for Delphi-oriented source code where the compiler and licensing conditions permit. The NUN project does not redistribute proprietary Delphi compiler binaries, proprietary RTL code, design-time packages or undocumented binary formats.

Useful language and framework concepts to support through adapters:

- units, classes, interfaces, generics and attributes;
- RTTI and reflection metadata for inspectable service objects;
- component/property/event conventions for user-space configuration tools;
- streams and structured serialization;
- package dependency metadata and explicit package loading;
- exception boundaries and managed-memory ownership.

Delphi runtime packages (for example BPL/DCU-related artifacts) are compiler- and platform-specific. They are not a portable kernel module format and must not be loaded as NUN kernel extensions. A NUN Delphi binding should use the stable C ABI or an explicitly versioned user-space bridge. Free Pascal compatibility is not proof of Delphi binary compatibility.

## 5. Shared binding surface

The first language binding surface should be small and typed:

| NUN service | C ABI concept | Language-facing wrapper |
|---|---|---|
| System information | versioned system-info record | `Nun.System` / `nun_system_info()` |
| Memory mapping | opaque region handle and attributes | `Nun.Memory` |
| Process creation | executable/package handle and capability set | `Nun.Process` |
| IPC | bounded message endpoint and typed payload | `Nun.IPC` |
| Storage | opaque volume/file handles | `Nun.Storage` |
| Device access | capability-scoped device handles | `Nun.Device` |
| Time | monotonic tick and documented frequency | `Nun.Time` |
| Diagnostics | structured event and error record | `Nun.Diagnostics` |

The service ABI must define ownership, lifetime, alignment, error codes, cancellation and privilege checks. It must not pass language-managed strings, exceptions, class instances or compiler-specific array descriptors directly across the kernel boundary.

## 6. Build, link and package policy

Each compiler profile declares:

- compiler identity and version range;
- target triple, CPU features and ABI revision;
- runtime libraries and their licenses;
- startup object and linker requirements;
- supported language subset and excluded features;
- C ABI conformance tests;
- debug-symbol format and stack-unwind support;
- reproducible build metadata and dependency hashes.

The build graph must reject incompatible object formats, calling conventions, exception-unwind models and runtime versions. A successful compilation is not sufficient: the resulting executable must pass ABI, memory, exception, I/O and process-launch tests.

## 7. Staged implementation

1. **C ABI freeze:** define syscall IDs, structs, handles, errors and ownership rules.
2. **C bindings:** host and emulator tests against the canonical C header.
3. **Fortran interoperability:** small `ISO_C_BINDING` examples for system info, buffers and numerical services.
4. **Free Pascal interoperability:** units wrapping the same C ABI; test strings, records, exceptions and cleanup boundaries.
5. **Delphi source adapter:** document supported language subset and C ABI wrappers using licensed toolchains.
6. **Runtime profiles:** implement user-space startup, allocation, I/O and exception support incrementally.
7. **Package and IDE integration:** compiler discovery, build manifests, diagnostics, debug symbols and package registry.

A feature is supported only when the compiler, runtime, NUN bindings and target tests agree. Language names in a roadmap or manifest do not constitute compiler support.
