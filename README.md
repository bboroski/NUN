<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/IMPLEMENTATION-DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111" /></p>
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

**Current status: SPECIFICATION ONLY**

У репозиторії поки немає вихідного коду ядра, boot path, linker script, build system, драйверів або тестів. Доданий бренд-ресурс README не є частиною runtime. NUN не є bootable OS і не призначений для завантаження на фізичне обладнання.

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

Перший кодовий етап має додати відтворювану збірку, linker map, serial smoke test і автоматизовану перевірку образу. До цього статус проєкту залишається специфікаційним.

## Контракт із NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) відповідає за майбутню перевірку kernel image та handoff-даних. NUN має незалежно перевіряти отриманий контекст перед ініціалізацією. Керування пам'яттю, scheduler і драйвери належать ядру, а не launcher-рівню.

## Сумісність із Windows-іграми

NUN закладає майбутню підсистему запуску Windows-ігор через окреме user-mode середовище сумісності: PE32/PE32+ loader, Win32/NT runtime, графічні адаптери Direct3D → Vulkan, аудіо, введення, ізольовані профілі та діагностику.

**Статус: SPECIFICATION ONLY.** Ця специфікація, JSON-схема й матриця не запускають `.exe`. У NUN ще немає ядра, менеджера процесів, PE-loader, Win32 API, Vulkan-драйвера або bootable image. Перевірка профілю не є перевіркою запуску гри.

- [Архітектура Windows Game Compatibility](docs/architecture/windows-game-compatibility.md)
- [Схема профілю гри](spec/windows-game-profile.schema.json)
- [Матриця можливостей і readiness gates](data/compatibility/windows-game-capability-matrix.tsv)

Перевірка метаданих: `python tools/validate_windows_game_compatibility.py`.


Перший перевірюваний компонент етапу 1 — host-side PE-інспектор. Він читає заголовки PE32/PE32+, архітектуру, підсистему, секції та імпортовані DLL, але не виконує файл і не є завантажувачем NUN.

```sh
python tools/inspect_windows_pe.py path/to/game.exe --json
python -m unittest discover -s tests -v
```

## Екосистема

[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).



## Reaper-inspired engineering integration

- [Project-specific architecture mapping](docs/REAPER_DOMAIN_INTEGRATION.md)
- [Canonical 50-pattern catalogue in SAMBU](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)

## Closed-domain metric and boundary model

- [Project-specific boundary and invariant mapping](docs/CLOSED_DOMAIN_METRIC_INTEGRATION.md)
- [Canonical CDM-1 specification and validator](https://github.com/bboroski/SAMBU/blob/main/docs/CLOSED_DOMAIN_METRIC.md)
