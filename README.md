<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY%20%C2%B7%20DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111" alt="Specification only · documentation scaffold"></p>
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

> **NUN** — NUN — специфікаційна основа майбутнього мінімального системного ядра. Репозиторій визначає архітектурні, boot, memory та execution contracts.

## Поточні можливості
- Boot contract і versioned handoff від NUNLauncher.
- Architecture layer: CPU entry, privilege transitions, exceptions та interrupt vectors.
- Memory model: physical pages, early allocator, virtual addressing і protection.
- Execution model, synchronization, device boundaries та diagnostics.
- Readiness gates для reproducible build, emulator і target qualification.

## Швидкий старт і перевірка
Перевірте документацію та контракти репозиторію перед реалізацією; повний build/boot pipeline наразі не заявлено.

## Архітектура, межі та достовірність
NUN наразі не є завантажуваною ОС: немає підтвердженого kernel boot path або драйверного стека. Невдалі integration checks у відкритих PR не включені в main і не подаються як реалізовані функції.

## Інтерфейс і візуальні матеріали

NUN наразі є специфікаційною основою ядра, а не графічним застосунком. GUI-скриншотів немає; зображення нижче демонструє лише спільну візуальну мову документації Duck Black / Yellow.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Спільний дизайн Duck Black Yellow; не інтерфейс NUN" width="960"></p>

## Інженерна якість
- Перевіряйте збірку й тести саме для цільової платформи; host CI не замінює hardware qualification.
- Статуси **implemented**, **partial**, **planned** мають підтверджуватися кодом, тестами та документацією.
- Не оголошуйте функцію production-ready лише на підставі макета, специфікації чи відкритого Pull Request.
- Зовнішні інтеграції мають бути явними, версіонованими, ізольованими й перевірюваними.

## Карта документації

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


## Kernel Architecture Explorer

The [NUN Architecture Explorer](docs/gui/index.html) and [1600×900 vector preview](docs/gui/preview.svg) provide a navigable view of boot, CPU, memory and execution contracts. **NUN remains a specification/documentation scaffold; this interface does not boot a kernel and is not evidence of a working operating system.**

The prototype is responsive, keyboard-operable and reduced-motion aware. See [capability composition](docs/architecture/capability-composition.md), [ecosystem integration](docs/ECOSYSTEM_INTEGRATION.md) and [technical debt register](docs/TECHNICAL_DEBT_REGISTER.md).