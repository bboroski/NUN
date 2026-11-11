<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY%20%C2%B7%20DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111" alt="Specification only · documentation scaffold"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

> **NUN** — NUN — специфікаційна основа майбутнього мінімального системного ядра. Репозиторій визначає архітектурні, boot, memory та execution contracts.

## Поточні можливості
- Boot contract і versioned handoff від NUNLauncher.
- Architecture layer: CPU entry, privilege transitions, exceptions та interrupt vectors.
- Memory model: physical pages, early allocator, virtual addressing і protection.
- Execution model, synchronization, device boundaries та diagnostics.
- Readiness gates для reproducible build, emulator і target qualification.

## Швидкий старт і перевірка
```bash
Перевірте документацію та контракти репозиторію перед реалізацією; повний build/boot pipeline наразі не заявлено.
```

## Архітектура, межі та достовірність
NUN наразі не є завантажуваною ОС: немає підтвердженого kernel boot path або драйверного стека. Невдалі integration checks у відкритих PR не включені в main і не подаються як реалізовані функції.

## Інженерна якість
- Перевіряйте збірку й тести саме для цільової платформи; host CI не замінює hardware qualification.
- Статуси **implemented**, **partial**, **planned** мають підтверджуватися кодом, тестами та документацією.
- Не оголошуйте функцію production-ready лише на підставі макета, специфікації чи відкритого Pull Request.
- Зовнішні інтеграції мають бути явними, версіонованими, ізольованими й перевірюваними.

## NUN Mesh та пов'язані репозиторії
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) · [Luci](https://github.com/bboroski/Luci)

Репозиторії зберігають власні назви, код, API, цикли випуску та межі відповідальності. Інтеграція означає узгоджені контракти, а не автоматичне злиття незалежних runtime.

## Дизайн і документація
Базовий інтерфейсний напрям екосистеми — **Duck Black / Yellow**: чорні поверхні, жовті акценти, читабельна типографіка, доступний контраст, клавіатурна навігація та підтримка reduced motion. Це візуальна рекомендація для документації й майбутніх інструментів, а не твердження про наявність GUI в NUN.

## Ліцензія
Ліцензію не оголошено; файл LICENSE відсутній.

---
**Створено з любов'ю до інженерії, системності та людей.**


## Майбутній GUI та внесок

NUN наразі є специфікацією, а не завантажуваною ОС або GUI. Вимоги до майбутніх host/developer tools та межі доказів описані в [GUI Design Standard](docs/gui/GUI_DESIGN_STANDARD.md). Макетні статуси не є kernel telemetry. Шаблон PR: [.github/PULL_REQUEST_TEMPLATE.md](.github/PULL_REQUEST_TEMPLATE.md).
