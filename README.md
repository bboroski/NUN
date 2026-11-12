<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-EARLY%20x86_64%20KERNEL%20BRING--UP%20%C2%B7%20NOT%20A%20COMPLETE%20OS-FFD600?style=for-the-badge&labelColor=111111" alt="Early x86_64 kernel bring-up · not a complete OS"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Оновлено: 2026-11-11</sub>


## Навігація

- [Поточні можливості](#поточні-можливості)
- [Швидкий старт і перевірка](#швидкий-старт-і-перевірка)
- [Додатковий Limine boot target](#додатковий-limine-boot-target)
- [Архітектура, межі та достовірність](#архітектура-межі-та-достовірність)
- [Інтерфейс і візуальні матеріали](#інтерфейс-і-візуальні-матеріали)
- [Інженерна якість](#інженерна-якість)
- [Карта документації](#карта-документації)
- [NUN Mesh та пов'язані репозиторії](#nun-mesh-та-повязані-репозиторії)
- [Дизайн і документація](#дизайн-і-документація)
- [Ліцензія](#ліцензія)

> **NUN** — NUN — специфікаційна основа майбутнього мінімального системного ядра. Репозиторій визначає архітектурні, boot, memory та execution contracts.

## Поточні можливості
- Boot contract і versioned handoff від NUNLauncher.
- Architecture layer: CPU entry, privilege transitions, exceptions та interrupt vectors.
- Memory model: physical pages, early allocator, virtual addressing і protection.
- Execution model, synchronization, device boundaries та diagnostics.
- Readiness gates для reproducible build, emulator і target qualification.

## Швидкий старт і перевірка
Перший x86_64 Multiboot2 bring-up target збирається окремим скриптом: `bash scripts/build-qemu.sh`, запуск — `bash scripts/run-qemu.sh`. Автоматична перевірка Multiboot2 bring-up і serial diagnostics виконується workflow `NUN QEMU kernel bring-up`.

## Додатковий Limine boot target

Окремий `make iso` збирає гібридний BIOS/x86_64 UEFI образ через Limine 12.9.6 із закріпленою SHA-256 перевіркою архіву. `make test` запускає host-тести маніфесту, `make run` запускає образ у QEMU. Цей Limine target поки окремий від Multiboot2 bring-up та не використовує його allocator/interrupt stack.

`make usb DEVICE=/dev/sdX` записує образ на цілий USB-диск. Операція знищує всі дані на вибраному диску, вимагає root і точного підтвердження `ERASE /dev/...`; інсталятор відхиляє partition, read-only або mounted targets і системний root-диск. Перевірте модель/розмір пристрою та зробіть резервну копію перед запуском.

Доказом для Limine target є QEMU smoke-test через BIOS та x86_64 UEFI. Це ранній x86_64 boot prototype, а не повноцінна ОС чи кваліфікація фізичного обладнання. E2K-компонент у цьому PR — окремий Linux userspace runtime під MCST LCC; він не є bare-metal E2K kernel і потребує окремого native E2K runner.

## Архітектура, межі та достовірність
NUN має ранній x86_64 Multiboot2 boot path із serial-консоллю, обмеженим physical-page bitmap allocator, IDT exception stubs і PIT IRQ0. Додатковий незалежний Limine target формує BIOS/UEFI ISO та перевіряє початковий kernel entry у QEMU. Ці два bring-up paths залишаються окремими; повноцінний scheduler, SMP, ACPI, драйвери, VFS, мережа, userspace та GUI ще не реалізовані.

## Інтерфейс і візуальні матеріали

NUN наразі є специфікаційною основою ядра, а не графічним застосунком. GUI-скриншотів немає; зображення нижче демонструє лише спільну візуальну мову документації Duck Black / Yellow.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Спільний дизайн Duck Black Yellow; не інтерфейс NUN" width="960"></p>



### Версіонований boot-info контракт

Додатково реалізовано host-tested C11 контракт передачі boot-метаданих: `include/nun/boot_info.h` та `src/boot_info.c` перевіряють версію структури, ідентифікатор архітектури, межі образу ядра, entry point і карту пам’яті. Це перевірка вхідних даних; вона не замінює завантажувач, перехід до ядра, тестування в емуляторі чи кваліфікацію на цільовому обладнанні.

```sh
cmake -S . -B build -DNUN_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Інженерна якість
- Перевіряйте збірку й тести саме для цільової платформи; host CI не замінює hardware qualification.
- Статуси **implemented**, **partial**, **planned** мають підтверджуватися кодом, тестами та документацією.
- Не оголошуйте функцію production-ready лише на підставі макета, специфікації чи відкритого Pull Request.
- Зовнішні інтеграції мають бути явними, версіонованими, ізольованими й перевірюваними.

## Карта документації

- [QEMU Multiboot2 kernel bring-up](docs/QEMU_KERNEL_BRINGUP.md)
- [Limine boot and USB image](docs/BOOT.md)
- [Boot image manifest schema](spec/boot-image-manifest.schema.json)
- [E2K Linux userspace profile](docs/ELBRUS_E2K.md)
- [OS subsystem ownership](docs/architecture/OS_SUBSYSTEM_OWNERSHIP.md)
- [Композиція можливостей](docs/architecture/capability-composition.md)
- [Інтеграція екосистеми](docs/ECOSYSTEM_INTEGRATION.md)
- [Технічний борг і межі](docs/TECHNICAL_DEBT_REGISTER.md)

## NUN Mesh та пов'язані репозиторії
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher)

Репозиторії зберігають власні назви, код, API, цикли випуску та межі відповідальності. Інтеграція означає узгоджені контракти, а не автоматичне злиття незалежних runtime.

## Дизайн і документація
Базовий інтерфейсний напрям екосистеми — **Duck Black / Yellow**: чорні поверхні, жовті акценти, читабельна типографіка, доступний контраст, клавіатурна навігація та підтримка reduced motion. Це візуальна рекомендація для документації й майбутніх інструментів, а не твердження про наявність GUI в NUN.

## Ліцензія
Ліцензію не оголошено; файл LICENSE відсутній.

---
**Створено з любов'ю до інженерії, системності та людей.**

## Федерація Hellduck

Цей репозиторій входить до федерації [Hellduck](https://github.com/bboroski/Hellduck) — спільного каталогу проєктів, контрактних зв’язків і станів інтеграції. Вихідний код, API, тести та цикл випуску залишаються відповідальністю цього репозиторію; Hellduck індексує зв’язки й не завантажує та не виконує код учасників. Практичні сценарії взаємодії, типи адаптерів і межі сумісності: [Integration Playbook](https://github.com/bboroski/Hellduck/blob/main/docs/INTEGRATION_PLAYBOOK.md).

## Owner working profile & engineering translation

Project-facing working preferences, engineering principles, definition of done and repository-specific integration boundaries: [docs/OWNER_WORKING_PROFILE.md](docs/OWNER_WORKING_PROFILE.md).
