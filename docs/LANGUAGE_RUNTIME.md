# NUN language runtime platform: Fortran, Pascal and Delphi

## 1. Common operating-system contract

NUN exposes one stable system ABI to every language family. Compilers do not call kernel internals directly. The language runtime maps portable library operations onto versioned user-space services and syscalls.

Initial target: x86-64 ELF64, System V C ABI, fixed-width types, explicit pointer/length pairs and versioned structures. The kernel must not depend on a language-specific object layout, exception frame, string representation or allocator.

## 2. Fortran profile

Target GNU Fortran first. Preserve modules, derived types, generic interfaces, kind-selected precision, array expressions, pure/elemental procedures, IEEE arithmetic and stream I/O through the user-space runtime. Parallel adapters such as OpenMP are enabled only after the scheduler/thread ABI exists.

C interoperability uses ISO_C_BINDING and BIND(C). Fortran argument passing, character strings, array dimensions and derived-type layout are not interchangeable with C by assumption. Use explicit VALUE, C_PTR, C_CHAR, C_INT and interoperable BIND(C) declarations where appropriate.

Kernel policy: no libgfortran, I/O runtime, implicit allocation or Fortran main program in ring 0. A restricted freestanding subset may be evaluated for numerical routines only after floating-point state is explicitly configured and saved.

## 3. Pascal profile

Use Free Pascal as the initial open compiler path. Provide profiles for FPC, OBJFPC, TP, DELPHI and DELPHIUNICODE modes; units, records, enumerations, sets, pointers, procedural variables, generics, classes, interfaces, exceptions, properties, RTTI and initialization/finalization.

The RTL and target port must provide memory management, strings, exceptions, file access, threading and process APIs. Delphi mode is language compatibility, not a guarantee that every Delphi RTL package or binary is ABI-compatible.

## 4. Delphi/Object Pascal profile

NUN's Delphi profile aims at source compatibility and migration support, not reimplementation or redistribution of proprietary Embarcadero tools.

The compatibility plan includes Object Pascal constructs, generics, attributes, RTTI, exception semantics, deterministic package linking, import of DFM/FMX resources as a tooling layer, a NUN component/event model and source checks for unsupported VCL/FMX APIs.

VCL and FireMonkey are not promised as drop-in NUN frameworks. The initial portable route is Free Pascal Delphi mode plus a NUN compatibility runtime. Embarcadero target availability remains controlled by Embarcadero.

## 5. Shared SDK packages

| Package | Responsibility |
|---|---|
| NUN.System | versioned ABI, process identity, capabilities and system information |
| NUN.Memory | bounded allocation, arena and ownership helpers |
| NUN.IO | files, streams, descriptors and cancellation |
| NUN.Threads | thread lifecycle, locks, wait primitives and monotonic deadlines |
| NUN.Devices | device discovery and typed capability handles |
| NUN.Network | sockets and transport-independent network services |
| NUN.UI | widgets, events, accessibility and resource loading |
| NUN.Data | structured reports, database adapters and serialization |
| NUN.Interop | C ABI, FFI declarations and ABI version checks |

These are proposed SDK contracts, not delivered libraries yet.

## 6. Compatibility stages

| Stage | Fortran | Pascal | Delphi |
|---|---|---|---|
| L0 source/tooling | syntax and project recognition | syntax and mode recognition | source/mode recognition |
| L1 host integration | gfortran host programs | FPC supported modes | FPC Delphi-mode subset |
| L2 NUN C ABI | BIND(C) procedures and data | cdecl exports/imports and fixed records | cdecl-compatible subset |
| L3 NUN user space | compiler target and runtime | target RTL, heap and exceptions | compatibility RTL |
| L4 app platform | scientific and parallel libraries | units, packages and GUI components | DFM/FMX import, events, RTTI and migration |

No stage is complete merely because a file parses. It requires compiler, linker, runtime and target test evidence.

## 7. Example ABI declarations

Fortran:

~~~fortran
module nun_system
  use, intrinsic :: iso_c_binding
  implicit none
  interface
    integer(c_int) function nun_system_abi_version() bind(C)
      import :: c_int
    end function
  end interface
end module
~~~

Pascal:

~~~pascal
unit Nun.System;
{$mode delphi}
interface
function NunSystemAbiVersion: LongInt; cdecl; external 'libnun';
implementation
end.
~~~

Delphi source compatibility uses the same stable C entry points; it does not expose kernel-private language objects across the boundary.

## 8. Runtime separation

Compiler processes execute in user space. Generated programs run as isolated processes when NUN's process/memory model exists. Language exceptions never unwind through kernel frames. Runtime allocators own user memory, not physical page tables. I/O and device access use capability-checked services. Compiler/package downloads require authenticated signatures and explicit approval.

**Non-goal:** loading arbitrary Fortran/Pascal/Delphi modules as kernel extensions. Kernel modules need a separate signed native ABI and strict review.
