# Semantic Autonomy: Naming and Migration Contract

**Status:** proposal for review  
**Revision:** 1  
**Applies to:** NUN  
**Branch:** feat/semantic-autonomy-2026-11-11

## Purpose

This contract establishes a controlled policy for choosing clear native-language names for concepts exposed by this project. It aims for semantic independence in user-facing language without sacrificing interoperability, discoverability, or compatibility.

This is a naming and migration specification, not evidence that the codebase has already been translated or mechanically renamed. No production identifier is changed by this document alone.

## 1. Core principles

1. **Meaning before purity.** Prefer a precise native-language term when it is clear to the intended audience. Never replace a precise technical term with an ambiguous invention merely to avoid borrowing.
2. **Endogenous naming.** New product concepts and user-facing labels should be coined from native roots or concise descriptive phrases. Record definition, scope, and examples.
3. **One concept, one canonical name.** The project glossary is authoritative. Synonyms may remain searchable, but each interface context has one preferred label.
4. **Context-aware names.** An operation is named as an action, a resource as a thing, and a status as a state.
5. **Interoperability is mandatory.** Do not rename standards-defined fields, wire-protocol tokens, file formats, command-line flags, environment variables, third-party symbols, or externally consumed API fields without an explicit migration plan and all-consumer review.
6. **No false promises.** A glossary entry or migration plan is not an implemented feature. Distinguish proposed, accepted, experimental, deprecated, and supported states.

## 2. Naming layers

| Layer | Policy |
|---|---|
| User interface | Prefer native-language labels; retain a familiar technical term in help text where useful |
| Documentation | Define the preferred term and include the conventional term once for searchability |
| Internal code identifiers | Preserve existing identifiers; new names follow the repository's established style |
| Public APIs and data formats | Preserve published names and schemas; introduce aliases only through a versioned compatibility plan |
| External standards and dependencies | Preserve canonical spelling, capitalization, and wire representation |

## 3. Initial glossary

These are candidate labels, not unconditional replacements:

| Conventional term | Preferred label | Boundary |
|---|---|---|
| interface | сопряжение; поверхность взаимодействия | Boundary through which components exchange actions or data |
| bug | ошибка; сбой | «Ошибка» for a defect; «сбой» for an observed failure |
| feature | возможность; функция | User-visible capability or callable behavior |
| deploy | ввод в действие; развёртывание | Moving a verified build into a target environment |
| commit | фиксация изменения | A version-control record; do not rename the Git command |
| repository | хранилище исходных материалов | Project history and files managed by version control |
| token | ключ; маркер | «Ключ» only for credentials; «маркер» for parser/protocol units |
| cloud | удалённая среда; распределённая среда | Choose according to actual topology |
| stream | поток | Ordered sequence of data or events |
| link | связь; ссылка | «Связь» for a relation; «ссылка» for a navigable reference |
| client | потребитель; запрашивающая сторона | Only when it describes the component's actual role |
| endpoint | конечная точка; адрес операции | Distinguish network address from operation |
| runtime | среда исполнения | Execution environment, not the process itself |
| build | сборка | Produced artifact or its production process |
| test | проверка; испытание | Automated assertion or broader validation |

## 4. Term proposal record

Every proposed term must include: stable concept ID; current term and source location; preferred label; one-sentence definition and non-meanings; affected layers; alternatives and rejection reasons; compatibility impact; evidence from files/tests/schemas/consumers; decision state; owner and review date.

Inspect call sites, schemas, tests, and consumers. Never infer meaning from spelling alone.

## 5. Migration sequence

A. **Inventory:** scan source, docs, UI resources, examples, schemas, generated files, and API descriptions. Classify by meaning and layer; exclude vendored/generated artifacts from direct editing.

B. **Semantic review:** group occurrences by concept, not matching text. Resolve homonyms and false positives. Map externally visible names.

C. **Parallel naming:** introduce accepted labels in docs and user-facing surfaces. Show the conventional term parenthetically during transition when useful.

D. **Compatible adoption:** use language-aware refactoring for internal symbols. For public interfaces, prefer versioned aliases, adapters, or deprecation notices. Never silently change serialized fields or command syntax.

E. **Removal review:** remove an old alias only after usage evidence, a published migration window, passing compatibility tests, and explicit maintainer approval. Preserve historical release notes.

## 6. Safety and correctness gates

- Meaning is unchanged and the new label is unambiguous.
- Unicode normalization, case, locale, and transliteration behavior are defined.
- Imports, reflection, serialization, plugin discovery, configuration, scripts, and generated references are checked.
- Public API and data compatibility are tested against supported versions.
- Docs, links, examples, CLI help, UI accessibility labels, and localization resources are updated.
- A rollback path exists.
- Credentials, protocol keywords, vendor-owned identifiers, and third-party license text are never rewritten.
- Every automated change is reviewed as a diff; blind repository-wide substitution is forbidden.

A failed gate blocks migration. Record the reason rather than forcing a replacement.

## 7. Validation plan

This first stage is documentation-only. Before executable renames, add applicable tests: glossary schema validation; inventory checks with allowlists for standards/external symbols; renamed-symbol/import/call-site tests; old/new API compatibility tests; UI snapshot/localization/accessibility checks; and the full supported build, lint, and test suite.

Do not report tests as passing unless they were actually run. If no test harness exists, state that and define a manual review.

## 8. Governance

- Review glossary changes like source code.
- Reject terms that are awkward, misleading, hard to pronounce, or conflict with established usage.
- Native-language naming must not hide conventional terms from contributors or interoperability partners.
- Existing code-identifier conventions remain until a separate tested language policy is approved.
- Each repository owns its local vocabulary; shared concepts use consistent definitions across the ecosystem.
- Naming policy does not alter licenses, authorship, provenance, copyright, or trademark rights.

## 9. Acceptance criteria

- [ ] Maintainers approve glossary and naming layers.
- [ ] Repository-specific terms are mapped to real source locations.
- [ ] Public interfaces and external dependencies are identified.
- [ ] Each executable migration has tests and rollback.
- [ ] No breaking rename is merged without an explicit versioned decision.

**Current implementation state:** specification only. Adding this document changes no source-code, API, schema, command, or UI names.
