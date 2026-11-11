<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><strong>Только спецификация · документационный каркас</strong></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

> **NUN** — NUN — спецификационная основа будущего минимального системного ядра, определяющая архитектурные, boot, memory и execution contracts.

## Текущие возможности
- Версионированный boot handoff contract с NUNLauncher.
- Architecture layer: CPU entry, privilege transitions, exceptions и interrupt vectors.
- Требования к memory model, раннему allocator, виртуальной адресации и защите.
- Readiness gates для воспроизводимой сборки, эмулятора и целевой платформы.

## Быстрый старт и проверка
```bash
Изучите спецификации и контракты перед реализацией; полноценный build/boot pipeline пока не заявлен.
```

## Архитектура и подтверждённые границы
NUN пока не является загружаемой ОС. Подтверждённый kernel boot path и стек драйверов отсутствуют. Проваленные integration checks открытых PR не входят в main и не считаются реализованными функциями.

## Инженерное качество
- Запускайте проверки для целевой платформы; host CI не заменяет аппаратную квалификацию.
- Статусы implemented, partial и planned должны подтверждаться кодом, тестами и доказательствами.
- Макет, спецификация или открытый Pull Request не доказывают production-ready реализацию.
- Внешние интеграции должны быть явными, версионированными, изолированными и тестируемыми.

## NUN Mesh и связанные репозитории
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) · [Luci](https://github.com/bboroski/Luci)

Каждый репозиторий сохраняет каноническое имя, собственный код, API, цикл выпуска и границы ответственности. Интеграция означает версионированные контракты, а не автоматическое слияние независимых runtime.

## Дизайн и документация
Общее направление интерфейсов экосистемы — Duck Black / Yellow: тёмные поверхности, жёлтые акценты, читаемая типографика, доступный контраст, клавиатурная навигация и поддержка reduced motion. Это визуальная рекомендация для документации и будущих инструментов, а не утверждение о наличии GUI в NUN.

## Лицензия
Лицензия не объявлена; файл LICENSE отсутствует.

---
**Создано с любовью к инженерии, системности и людям.**


## GUI и правила внесения изменений

Общая визуальная основа: глубокий чёрный `#111111` и жёлтый Duck `#FFD600`. Границы GUI, различие реального интерфейса и концепт-макета, доступность и требования к доказательствам описаны в [GUI Design Standard](docs/gui/GUI_DESIGN_STANDARD.md). Для изменений используйте [шаблон Pull Request](.github/PULL_REQUEST_TEMPLATE.md); перед слиянием требуются два независимых одобрения и успешные обязательные CI-проверки при включённой защите ветки.
