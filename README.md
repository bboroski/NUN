<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-C11%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/IMPLEMENTATION-HOST%20METADATA%20SLICE-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**Специфікаційна основа мінімального системного ядра: межі архітектури, пам'яті, виконання та передачі керування.**

## Продуктова роль

NUN фіксує технічний контракт майбутнього мінімального ядра до появи повної реалізації. Це точка узгодження для системних розробників: що саме має робити kernel core, які дані він отримує від bootstrap-рівня та які гарантії мають бути підтверджені тестами.

## Запропоновані межі системи

- **Boot contract:** формат handoff від завантажувача, карта пам'яті та версія ABI.
- **Architecture layer:** CPU entry, privilege transition, exception та interrupt vectors.
- **Memory layer:** фізичні сторінки, ранній allocator, віртуальна адресація й захист.
- **Execution layer:** task/context model та явна модель синхронізації.
- **Device boundary:** описані bus/driver interfaces без припущень про конкретну плату.
- **Diagnostics:** ранній serial/framebuffer output і структурований panic record.

## Поточний стан

**Поточний стан: додано C11-контракт перевірки boot-інформації; реальне ядро ще не реалізоване.**

Репозиторій тепер містить функцію nun_boot_info_validate, версійовану структуру handoff, перевірку архітектурного ID, карти пам'яті, переповнення діапазонів, перетинів і entry point, CMake та host-тести (python one.py check). Перевіряються лише метадані на host-платформі. Тут досі немає CPU entry, linker script, page allocator, scheduler, драйверів чи завантажуваного kernel image. NUN не є bootable OS.

## Інженерні вимоги

Кожна апаратно-залежна дія має бути прив'язана до конкретної архітектури й boot protocol. ABI, межі пам'яті, alignment, privilege level та interrupt policy потрібно документувати явно. Host-тести не можуть замінити емуляцію або перевірку на цільовій платформі.

## Управління розробкою та readiness gates

| Етап | Необхідний доказ |
|---|---|
| Architecture baseline | Обрана CPU architecture і визначена модель привілеїв |
| Boot contract | Версійований handoff, memory map та правила валідації |
| Reproducible build | Збірка, linker map та перевірка образу відтворюються |
| Emulator validation | Serial smoke test, exception/interrupt та memory tests |
| Target qualification | Фізичне обладнання й конкретний board port перевірені окремо |

Наступні етапи: architecture entry, linker map, раннє виведення діагностики, фізична/віртуальна пам'ять та окремі емуляторні й target-тести. Host-контракт не замінює жоден із цих етапів.

## Контракт із NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) відповідає за майбутню перевірку kernel image та handoff-даних. NUN має незалежно перевіряти отриманий контекст перед ініціалізацією. Керування пам'яттю, scheduler і драйвери належать ядру, а не launcher-рівню.

## Екосистема

[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).



## Reaper-inspired engineering integration

- [Project-specific architecture mapping](docs/REAPER_DOMAIN_INTEGRATION.md)
- [Canonical 50-pattern catalogue in SAMBU](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)
