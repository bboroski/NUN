<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><strong>Код фазы A добавлен · проверка QEMU ожидает CI</strong></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Обновлено: 2026-11-11</sub>


## Навигация

- [Текущие возможности](#текущие-возможности)
- [Быстрый старт и проверка](#быстрый-старт-и-проверка)
- [Архитектура и подтверждённые границы](#архитектура-и-подтверждённые-границы)
- [Интерфейс и визуальные материалы](#интерфейс-и-визуальные-материалы)
- [Инженерное качество](#инженерное-качество)
- [Карта документации](#карта-документации)
- [NUN Mesh и связанные репозитории](#nun-mesh-и-связанные-репозитории)
- [Дизайн и документация](#дизайн-и-документация)
- [Лицензия](#лицензия)
- [Палитра Duck Black / Yellow](#палитра-duck-black-yellow)

> **NUN** — NUN — минимальная основа системного ядра; первая реализация x86_64 bring-up находится в отдельной ветке, подтверждение загрузки в QEMU ожидает CI.

## Текущие возможности
- Multiboot2 entry для x86_64 и переход в long mode.
- Диагностическая serial-консоль COM1 и проверка memory map из Multiboot2.
- Ранний allocator физических страниц 4 KiB с резервированием ядра и boot-info.
- Базовые exception gates в IDT, remap PIC и таймерный IRQ от PIT.
- QEMU boot gate добавлен; выполнение CI ещё ожидается.

## Быстрый старт и проверка
Требуются GCC, GNU binutils, Make, GRUB, xorriso и QEMU: `make clean iso && make qemu-test`. Полноценный production boot pipeline пока не заявлен.

## Архитектура и подтверждённые границы
Первый kernel boot path добавлен в `feature/nun-integration-phase-a`, но QEMU smoke test ещё не подтверждён CI. Это ограниченный x86_64 bring-up, а не полноценная ОС; драйверы, userspace и системные службы пока не реализованы.

## Интерфейс и визуальные материалы

NUN сейчас является спецификационной основой ядра, а не графическим приложением. Скриншотов GUI нет; изображение ниже показывает только общий стиль документации Duck Black / Yellow.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Общая дизайн-система Duck Black Yellow; не интерфейс NUN" width="960"></p>

## Инженерное качество
- Запускайте проверки для целевой платформы; host CI не заменяет аппаратную квалификацию.
- Статусы implemented, partial и planned должны подтверждаться кодом, тестами и доказательствами.
- Макет, спецификация или открытый Pull Request не доказывают production-ready реализацию.
- Внешние интеграции должны быть явными, версионированными, изолированными и тестируемыми.

## Карта документации

- [QEMU bootstrap phase A](docs/QEMU_BOOTSTRAP.md)
- [Карта интеграции 12 репозиториев](docs/NUN_INTEGRATION.md)
- [Changelog NUN](CHANGELOG.NUN)
- [Технический долг и границы](docs/TECHNICAL_DEBT_REGISTER.md)

## NUN Mesh и связанные репозитории
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher)

Каждый репозиторий сохраняет каноническое имя, собственный код, API, цикл выпуска и границы ответственности. Интеграция означает версионированные контракты, а не автоматическое слияние независимых runtime.

## Дизайн и документация
Общее направление интерфейсов экосистемы — Duck Black / Yellow: тёмные поверхности, жёлтые акценты, читаемая типографика, доступный контраст, клавиатурная навигация и поддержка reduced motion. Это визуальная рекомендация для документации и будущих инструментов, а не утверждение о наличии GUI в NUN.

## Лицензия
Лицензия не объявлена; файл LICENSE отсутствует.

---
**Создано с любовью к инженерии, системности и людям.**

## Палитра Duck Black / Yellow
Совместимые цвета прежних badges и marks: `#FFD600` (жёлтый) и `#111111` (почти чёрный). Текущий фон интерфейса — `#0D0D0D`. Полная палитра из шести цветов, четырнадцати функциональных оттенков и значения RGB/HSL находятся в `design/duck-ui.tokens.json` (schema 3.0.0). Жёлтый зарезервирован для основных действий, фокуса и выделения; красный и голубой обозначают семантические ошибки и информационные состояния.
