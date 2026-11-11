<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-PHASE%20A%20CODE%20ADDED%20%C2%B7%20QEMU%20VERIFICATION%20PENDING-FFD600?style=for-the-badge&labelColor=111111" alt="Specification only · documentation scaffold"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Оновлено: 2026-11-11</sub>


## Навігація

- [Поточні можливості](#поточні-можливості)
- [Швидкий старт і перевірка](#швидкий-старт-і-перевірка)
- [Архітектура, межі та достовірність](#архітектура-межі-та-достовірність)
- [Інтерфейс і візуальні матеріали](#інтерфейс-і-візуальні-матеріали)
- [Інженерна якість](#інженерна-якість)
- [Карта документації](#карта-документації)
- [NUN Mesh та пов'язані репозиторії](#nun-mesh-та-повязані-репозиторії)
- [Дизайн і документація](#дизайн-і-документація)
- [Ліцензія](#ліцензія)

> **NUN** — NUN — мінімальна системна основа з реалізацією першого x86_64 bring-up етапу на окремій гілці; завантаження в QEMU очікує CI-підтвердження.

## Поточні можливості
- Multiboot2 x86_64 entry та перехід у long mode.
- COM1 serial diagnostics і перевірка boot-information memory map.
- Ранній allocator сторінок 4 KiB із резервуванням ядра та boot-info.
- IDT для базових exception vectors, PIC remap і PIT timer IRQ.
- QEMU boot gate додано; результат очікує виконання CI.

## Швидкий старт і перевірка
Потрібні GCC, GNU binutils, Make, GRUB, xorriso та QEMU: `make clean iso && make qemu-test`. Повний production boot pipeline наразі не заявлено.

## Архітектура, межі та достовірність
Перший kernel boot path реалізовано в `feature/nun-integration-phase-a`, але його QEMU smoke test ще не підтверджено CI. Наразі це вузький x86_64 bring-up, а не повноцінна ОС; драйверний стек, userspace і системні служби ще не реалізовані.

## Інтерфейс і візуальні матеріали

NUN наразі є специфікаційною основою ядра, а не графічним застосунком. GUI-скриншотів немає; зображення нижче демонструє лише спільну візуальну мову документації Duck Black / Yellow.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Спільний дизайн Duck Black Yellow; не інтерфейс NUN" width="960"></p>

## Інженерна якість
- Перевіряйте збірку й тести саме для цільової платформи; host CI не замінює hardware qualification.
- Статуси **implemented**, **partial**, **planned** мають підтверджуватися кодом, тестами та документацією.
- Не оголошуйте функцію production-ready лише на підставі макета, специфікації чи відкритого Pull Request.
- Зовнішні інтеграції мають бути явними, версіонованими, ізольованими й перевірюваними.

## Карта документації

- [QEMU bootstrap phase A](docs/QEMU_BOOTSTRAP.md)
- [Карта інтеграції 12 репозиторіїв](docs/NUN_INTEGRATION.md)
- [Changelog NUN](CHANGELOG.NUN)
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

Цей репозиторій входить до федерації [Hellduck](https://github.com/bboroski/Hellduck) — спільного каталогу проєктів, контрактних зв’язків і станів інтеграції. Вихідний код, API, тести та цикл випуску залишаються відповідальністю цього репозиторію; Hellduck індексує зв’язки й не завантажує та не виконує код учасників.
