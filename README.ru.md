<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>NUN OS · Bare-metal Kernel Foundation</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-FOUNDATION%20IN%20PROGRESS-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/BOOT-HOST%20CONTRACT%20TESTS-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**NUN — основа самостоятельной операционной системы: загрузочный контракт, архитектурные порты, управление памятью, выполнение задач, драйверы, изоляция и пользовательская среда.**

## Архитектура продукта

NUN OS — отдельная bare-metal операционная система. NUNLauncher — самостоятельное Android-приложение типа Home, а не UEFI-загрузчик. Android-приложение не может самостоятельно заменить загрузчик устройства или записывать защищённые разделы прошивки.

Начальная инженерная цель NUN OS — x86_64 + UEFI в QEMU/OVMF. AArch64 и RISC-V 64-bit предусмотрены как следующие отдельные порты, но пока не заявлены как реализованные.

## Что уже заложено в репозитории

- Версионированный C11 boot-information ABI v1.
- Проверка magic, версии ABI, целевой архитектуры и зарезервированных флагов.
- Валидация memory map: размер, количество, выравнивание, типы, переполнения и пересечения диапазонов.
- Проверка диапазонов kernel image/initrd и согласованности framebuffer-полей.
- Host-тесты и GitHub Actions workflow для строгой C11-компиляции.
- Архитектура NUN OS, загрузочная цепочка, модель интеграции экосистемы и языковые профили Fortran/Pascal/Delphi.

Это **не bootable OS**: EFI loader, linker script, kernel entry, page tables, scheduler, драйверы, userspace и готовый образ ещё не реализованы.

## Языковые подсистемы

Общий системный ABI будет доступен через отдельные binding/runtime-профили:

- **Fortran:** модули, числовые типы, массивы, научные вычисления и `ISO_C_BINDING`.
- **Pascal / Free Pascal:** units, records, sets, classes, generics, exceptions, RTTI и RTL-профили.
- **Delphi:** Object Pascal, компоненты, свойства, события, RTTI и C ABI interoperability при наличии лицензированного toolchain.

Компиляторы и runtimes будут пользовательскими подсистемами. Они не загружаются в привилегированное ядро.

## Экосистема

| Репозиторий | Системная роль |
|---|---|
| SAMBU | Языки, компиляторы, IDE и developer tooling |
| ReaperCripple | BSP, архитектурные адаптеры, bootstrap и rollback |
| Kloz | Arena memory, платформенные контракты и resource budgets |
| CO | Lifecycle и bounded execution |
| SaintSI | Health observation, heartbeat и CRC-32 |
| ToolNG | Диагностика, evidence-модель и отчёты |
| Klipot | Реестр модулей, схемы, dependency graph |
| NUNLauncher | Android Home Launcher и пользовательская оболочка |

Интеграция выполняется через версионированные интерфейсы и адаптеры. Прикладные и платформенно-зависимые функции не переносятся в kernel автоматически.

## Сборка и тесты текущего host-контракта

```bash
make check
make test
```

Эти команды проверяют только boot-info validator. Они не собирают EFI loader и не производят загрузочный образ.

## Техническая документация

- [NUN OS foundation](docs/NUN_OS_FOUNDATION.md)
- [Boot chain and image contract](docs/BOOT_CHAIN_AND_IMAGE_CONTRACT.md)
- [Fortran, Pascal and Delphi integration](docs/LANGUAGE_RUNTIME_INTEGRATION.md)
- [All-repository capability map](docs/ECOSYSTEM_CAPABILITY_MAP.md)
- [Implementation roadmap](docs/IMPLEMENTATION_ROADMAP.md)
- [Capability composition](docs/architecture/capability-composition.md)

## Release policy

Поддержка архитектуры, bootable-образа или безопасного обновления объявляется только после реализации и соответствующих тестов в эмуляторе и на конкретном целевом устройстве.
