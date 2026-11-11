<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Основа операционной системы</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-EARLY%20FOUNDATION-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/BOOT-NOT%20BOOTABLE-FFB000?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C11%20HOST%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<p align="center"><sub><a href="design/DUCK_UI_SYSTEM.md">Duck Black / Yellow UI standard</a> · <a href="assets/duck-ui/duck-ui-preview.svg">Vector UI preview</a></sub></p>

**NUN — основа самостоятельной операционной системы.** Проект переходит от архитектурной спецификации к проверяемым системным контрактам, загрузочной цепочке и ядру.

## Что уже есть

- Версионированный C11 boot-information ABI с фиксированными типами и идентификаторами архитектур.
- Host-валидатор magic, ABI, архитектуры, memory map, перекрытий, переполнения адресов, kernel entry, initrd и framebuffer.
- Host-тесты валидатора и базовая цель make test.
- JSON Schema для boot manifest и каталог возможностей девяти подключённых репозиториев.
- Архитектурная основа ОС, матрица целевых процессоров, security/recovery policy и этапы готовности.
- Контракты интеграции Kloz, CO, SaintSI, ReaperCripple, SAMBU, Klipot, ToolNG и NUNLauncher.

## Статус

<strong>EARLY FOUNDATION · NOT BOOTABLE</strong>

В репозитории пока нет firmware loader, linker script, kernel entry point, interrupt controller, scheduler, virtual-memory manager, filesystem, стека драйверов или загрузочного образа. Host-тесты проверяют только boot metadata и не доказывают загрузку в эмуляторе или на физическом оборудовании.

## Архитектура

- **NUN** владеет ядром, ABI, памятью, процессами, IPC, безопасностью и системными службами.
- **Kloz** предоставляет узкую переносимую платформенную основу; интеграция требует аудита и target-проверки.
- **CO** и **SaintSI** могут подключаться как сервисные компоненты, но не заменяют scheduler, watchdog или криптографию.
- **ReaperCripple** предоставляет точные BSP/board-профили и контролируемые bootstrap-процедуры.
- **SAMBU** предоставляет экспериментальную языковую среду и инструменты; native compiler ещё не готов к production.
- **Klipot** остаётся независимой контрольной плоскостью; **ToolNG** — Windows-диагностикой рабочей станции.

## Системные языки

Fortran, Pascal и Delphi/Object Pascal планируются как полноценные языки пользовательского пространства через версионированный C ABI, системные модули, runtime-адаптеры и библиотеки. Fortran использует ISO_C_BINDING/BIND(C); Pascal — FPC и C-compatible records; Delphi — отдельную source/runtime compatibility matrix. Частные runtime-объекты не пересекают границу ABI. SAMBU сохраняет собственный экспериментальный toolchain.

## Целевые платформы

Первый эталонный путь — x86_64 UEFI в QEMU/OVMF. AArch64 и RISC-V — следующие порты. Elbrus/E2K остаётся отдельным архитектурным направлением без подтверждённых boot evidence. Поддержка Apple/Intel/AMD определяется конкретной архитектурой, прошивкой и платой, а не только производителем процессора.

## Сборка и тесты

Требуются C11-компилятор и Make. Запуск: <code>make test</code>, очистка: <code>make clean</code>.

Тест проверяет переносимые программные инварианты. Он не заменяет QEMU/OVMF boot test, linker map, serial trace или проверку физической платы.

## Документация

- [OS Foundation](docs/OS_FOUNDATION.md)
- [Boot information ABI](include/nun/boot_info.h)
- [Boot manifest schema](spec/boot-info-v1.schema.json)
- [System capability catalog](spec/system-capability-catalog.json)
- [Capability composition](docs/architecture/capability-composition.md)

## Лицензия

В NUN отсутствует файл LICENSE, лицензия не объявлена. Перед распространением или включением кода других репозиториев необходимо отдельно проверить лицензионные условия каждого компонента.

---

**NUN — создано с любовью к системному программированию, надёжности и людям.**
