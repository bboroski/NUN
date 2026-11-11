<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-HOST%20TESTED%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/IMPLEMENTATION-C11%20HANDOFF%20VALIDATOR-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-NOT%20DECLARED-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**NUN — спецификационная основа будущего минимального системного ядра.** Репозиторий фиксирует границы архитектуры, памяти, исполнения и передачи управления до начала полной реализации.

## Целевые системные контракты

- **Boot contract:** версионированный handoff от launcher, memory map и правила проверки.
- **Architecture layer:** CPU entry, privilege transition, exception и interrupt vectors.
- **Memory layer:** физические страницы, ранний allocator, виртуальная адресация и защита.
- **Execution layer:** task/context model и явная синхронизация.
- **Device boundary:** bus/driver interfaces без предположений о конкретной плате.
- **Diagnostics:** ранний serial/framebuffer output и структурированный panic record.

## Текущий статус

В репозитории появился первый host-tested C11 boot-info contract: версионированная handoff-структура и проверка архитектуры, границ kernel image, entry point и memory map. Это **ещё не загружаемая ОС**: architecture entry, linker script, boot path, scheduler, драйверы и bootable binary отсутствуют. E2K указан только как идентификатор целевой архитектуры в контракте, а не как подтверждённая поддержка.

## Первый реализованный системный контракт

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

`include/nun/boot_info.h` и `src/boot_info.c` проверяют handoff-метаданные без доступа к оборудованию, allocation, relocation или передачи управления на entry point. Это host-tested основа, а не kernel loader.

## План реализации и readiness gates

| Этап | Необходимое доказательство |
|---|---|
| Architecture baseline | Определены CPU architecture, ABI и модель привилегий |
| Boot contract | Версионированные handoff, memory map и правила проверки |
| Reproducible build | Повторяемая сборка, linker map и проверка образа |
| Emulator validation | Serial smoke test, exception/interrupt и memory tests |
| Target qualification | Отдельная проверка конкретного процессора и платы |

Первый этап с кодом должен включать минимальную сборку, linker map, serial smoke test и автоматическую проверку образа. Host-тесты не заменяют эмулятор и физическую платформу.

## Контракт с NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) должен проверять kernel image и формировать handoff-данные. NUN обязан самостоятельно проверить полученный контекст до инициализации. Управление памятью, scheduler и драйверы относятся к ядру, а не к уровню launcher.

## Экосистема и лицензия

Связанные компоненты: [SAMBU](https://github.com/bboroski/SAMBU), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [ReaperCripple](https://github.com/bboroski/ReaperCripple) и [NUNLauncher](https://github.com/bboroski/NUNLauncher).

Файл <code>LICENSE</code> отсутствует; лицензия не объявлена. Определите условия до распространения.

---

**Создано с любовью к системному программированию и людям.**