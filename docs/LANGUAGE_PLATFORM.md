# NUN language platform: Fortran, Pascal and Delphi

## 1. Product rule

Fortran, Pascal and Delphi-oriented development are first-class **system capabilities** of NUN. The operating system must ship discoverable toolchain profiles, runtime services, package metadata, debugging hooks, documentation and SDK integration. A language badge or editor syntax mode alone does not satisfy this requirement.

The kernel ABI remains language-neutral. Compilers and runtimes execute as isolated user-mode tools and services; they do not get kernel privileges by virtue of their language.

## 2. Fortran — numerical and scientific computing

The Fortran profile is optimized for numerical workloads:
- modern Fortran modules, procedures, derived types and array semantics;
- compiler integration through GNU Fortran and/or LLVM Flang, selected per target;
- ISO_C_BINDING as the stable interoperability boundary to the NUN C ABI;
- BLAS/LAPACK-compatible numerical libraries, parallel compute adapters and reproducible scientific package recipes;
- debugger integration for symbols, source locations, array inspection and runtime diagnostics.

A future NUN numerical service may expose explicitly versioned operations for linear algebra, simulation and data processing. It must not silently copy unbounded arrays or hide large memory allocations.

## 3. Pascal — native typed application and systems development

The Pascal profile covers:
- procedural Pascal and Object Pascal source modules;
- units, interfaces/implementations, records, enumerations, sets, generics and classes where supported by the selected compiler;
- deterministic ownership options and explicit interop with the C ABI;
- RTL, file/stream, collections and text services;
- native application and service templates, testing and debugging.

Free Pascal is the open compiler path for initial bring-up. Exact compiler version, language mode, target backend and available RTL units are recorded in each toolchain manifest rather than assumed globally.

## 4. Delphi / Object Pascal — RAD and component platform

The Delphi profile elevates the productive application model into system tooling:
- visual form designer and component palette;
- declarative form resources and stable component identifiers;
- event-handler bindings with inspectable signatures;
- Object Inspector/property editing and RTTI-based metadata;
- Data Modules for non-visual resources;
- package discovery, dependency/version constraints and design-time/runtime separation;
- database, network and UI adapters with permission-aware service access;
- project templates, incremental build, debugger, profiler and deploy/package actions.

The initial compatibility route is source-level Object Pascal through Free Pascal and an NUN-owned component/runtime abstraction. Delphi project migration tools may translate project metadata and flag unsupported APIs.

**Compatibility boundary:** this plan does not include redistribution of proprietary Delphi compiler code, RTL, VCL/FMX implementation or third-party binaries. Source compatibility, API shims, form-resource conversion and binary compatibility are separate milestones and must be tested independently.

## 5. Common language services

All language profiles use common NUN developer services:

| Service | Contract |
|---|---|
| Toolchain manager | compiler version, target triple, ABI, linker and runtime package |
| Project model | source units, dependencies, build profiles and reproducible options |
| Package service | signed package metadata, dependency resolution and uninstall/rollback |
| Diagnostics | source spans, severity, stable codes and machine-readable output |
| Debug interface | process/thread handles, breakpoints, symbols and controlled memory inspection |
| Language server | completion, navigation, references, formatting and semantic diagnostics |
| Build service | sandboxed build actions, declared inputs/outputs and reproducible logs |
| UI component service | versioned component metadata, events, resources and permission requirements |

SAMBU remains the experimental language and developer-tooling project. Its workflows and IDE concepts can be integrated through adapters; NUN does not require embedding the Python interpreter in its kernel.

## 6. Cross-language ABI

The first shared boundary is the versioned C ABI in include/nun/abi.h.

Bindings must:
- use fixed-width integer fields and explicit structure sizes;
- state string encoding and ownership;
- define array length, stride and lifetime;
- convert exceptions/errors to stable NUN status values;
- avoid passing compiler-specific object layouts across module boundaries;
- keep allocation and deallocation within the same runtime unless a shared allocator contract is explicitly selected.

Fortran uses ISO_C_BINDING. Pascal uses C-compatible records and external declarations. Delphi-oriented packages use declared adapters, never guessed binary layouts.

## 7. Delivery sequence

1. Publish the C ABI and host conformance tests.
2. Build one compiler profile for a development host and validate C interop.
3. Package one Fortran numerical example and one Pascal service example.
4. Add Object Pascal form metadata and a minimal component runtime.
5. Add the visual designer, Object Inspector and event binding workflow.
6. Cross-compile the toolchains for the first qualified NUN target.
7. Add signed package delivery, debugger integration and reproducible system images.

Until each step lands with code and tests, the related capability is **planned**, not implemented.
