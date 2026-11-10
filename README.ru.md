<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/IMPLEMENTATION-DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111" /></p>
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

**Current status: SPECIFICATION ONLY**

В репозитории пока нет исходного кода ядра, boot path, linker script, build system, драйверов или тестов. Бренд-ресурс README не является runtime-кодом. NUN не является bootable OS и не предназначен для загрузки на физическое оборудование.

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

Первый этап реализации должен добавить воспроизводимую сборку, linker map, serial smoke test и автоматическую проверку образа. До этого проект остаётся спецификационным каркасом.

## Контракт с NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) должен проверять kernel image и handoff-данные. NUN обязан независимо проверять полученный контекст перед инициализацией. Управление памятью, scheduler и драйверы относятся к ядру, а не к launcher-уровню.

## Экосистема

[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).


## Продуктовая система

Единые правила дизайна Duck, трёхъязычной документации, управления возможностями, маркетинговых заявлений и выпусков: [docs/PRODUCT_SYSTEM.md](docs/PRODUCT_SYSTEM.md). Реестр на 10 000 ID — это ёмкость планирования, а не заявление о 10 000 готовых функциях.
