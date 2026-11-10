<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-C11%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/IMPLEMENTATION-HOST%20METADATA%20SLICE-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**Спецификационная основа минимального системного ядра: границы архитектуры, памяти, выполнения и передачи управления.**

## Позиционирование

NUN фиксирует технический контракт будущего минимального ядра до появления полноценной реализации. Это общая точка согласования для системных разработчиков: обязанности kernel core, данные от bootstrap-уровня и доказательства, которые потребуются для проверки архитектуры.

## Предлагаемые границы системы

- **Boot contract:** формат handoff от загрузчика, карта памяти и версия ABI.
- **Architecture layer:** CPU entry, privilege transition, exception и interrupt vectors.
- **Memory layer:** физические страницы, ранний allocator, виртуальная адресация и защита.
- **Execution layer:** task/context model и явная модель синхронизации.
- **Device boundary:** описанные bus/driver interfaces без предположений о конкретной плате.
- **Diagnostics:** ранний serial/framebuffer output и структурированный panic record.

## Текущий статус

**Текущий статус: добавлен C11-контракт проверки boot-информации; само ядро ещё не реализовано.**

В репозитории появились nun_boot_info_validate, версионируемая handoff-структура, проверка ID архитектуры, карты памяти, переполнения диапазонов, пересечений и entry point, CMake и host-тесты (python one.py check). Проверяются только метаданные на host-платформе. CPU entry, linker script, page allocator, scheduler, драйверов и загружаемого kernel image пока нет. NUN не является bootable OS.

## Инженерные требования

Каждое аппаратно-зависимое действие должно быть привязано к конкретной архитектуре и boot protocol. ABI, границы памяти, alignment, privilege level и interrupt policy необходимо документировать явно. Host-тесты не заменяют эмуляцию или проверку на целевой платформе.

## Управление проектом и readiness gates

| Этап | Требуемое доказательство |
|---|---|
| Architecture baseline | Выбрана CPU architecture и определена модель привилегий |
| Boot contract | Версионируемый handoff, memory map и правила проверки |
| Reproducible build | Сборка, linker map и проверка образа воспроизводимы |
| Emulator validation | Serial smoke test, exception/interrupt и memory tests |
| Target qualification | Физическое оборудование и конкретный board port проверены отдельно |

Следующие этапы: architecture entry, linker map, ранняя диагностика, управление физической и виртуальной памятью, а также отдельные тесты в эмуляторе и на целевом устройстве. Host-контракт не заменяет эти этапы.

## Контракт с NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) должен проверять kernel image и handoff-данные. NUN обязан независимо проверять полученный контекст перед инициализацией. Управление памятью, scheduler и драйверы относятся к ядру, а не к launcher-уровню.

## Экосистема

[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).
