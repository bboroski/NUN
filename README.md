<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/IMPLEMENTATION-DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-NOT%20DECLARED-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**NUN — специфікаційна основа майбутнього мінімального системного ядра.** Репозиторій фіксує межі архітектури, пам'яті, виконання та передачі керування до початку повної реалізації.

## Цільові системні контракти

- **Boot contract:** версійований handoff від launcher, memory map і правила перевірки.
- **Architecture layer:** CPU entry, privilege transition, exception та interrupt vectors.
- **Memory layer:** фізичні сторінки, ранній allocator, віртуальна адресація й захист.
- **Execution layer:** task/context model та явна синхронізація.
- **Device boundary:** bus/driver interfaces без припущень про конкретну плату.
- **Diagnostics:** ранній serial/framebuffer output і структурований panic record.

## Поточний стан

У репозиторії наразі немає вихідного коду ядра, boot path, linker script, build system, драйверів або тестів. NUN **не є завантажуваною ОС** і не призначений для запуску на фізичному обладнанні. Жодна архітектура, включно з Elbrus/E2K, не має статусу перевіреної підтримки.

## План реалізації та readiness gates

| Етап | Обов'язковий доказ |
|---|---|
| Architecture baseline | CPU architecture, ABI та модель привілеїв визначені |
| Boot contract | Версійовані handoff, memory map і правила валідації |
| Reproducible build | Повторювана збірка, linker map і перевірка образу |
| Emulator validation | Serial smoke test, exception/interrupt і memory tests |
| Target qualification | Окрема перевірка конкретної плати й процесора |

Перший етап із кодом має принести мінімальну збірку, linker map, serial smoke test та автоматичну перевірку образу. Host-тести не замінюють емулятор і фізичну платформу.

## Контракт із NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) має перевіряти kernel image та формувати handoff-дані. NUN повинен самостійно перевірити отриманий контекст до ініціалізації. Менеджер пам'яті, scheduler і драйвери належать ядру, а не launcher-рівню.

## Екосистема та ліцензія

Пов'язані компоненти: [SAMBU](https://github.com/bboroski/SAMBU), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [ReaperCripple](https://github.com/bboroski/ReaperCripple) і [NUNLauncher](https://github.com/bboroski/NUNLauncher).

Файл <code>LICENSE</code> відсутній; ліцензію не оголошено. Визначте умови до розповсюдження.

---

**Створено з любов'ю до системного програмування та людей.**