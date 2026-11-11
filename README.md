<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Operating System Foundation</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-EARLY%20FOUNDATION-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/BOOT-NOT%20BOOTABLE-FFB000?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C11%20HOST%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<p align="center"><sub><a href="design/DUCK_UI_SYSTEM.md">Duck Black / Yellow UI standard</a> · <a href="assets/duck-ui/duck-ui-preview.svg">Vector UI preview</a></sub></p>

**NUN — основа майбутньої самостійної операційної системи.** Проєкт рухається від архітектурної специфікації до перевірюваних системних контрактів, завантажувального ланцюга та ядра.

## Що вже є

- Версіонований C11 boot-information ABI з фіксованими типами та кодами архітектур.
- Host-валидатор magic, ABI, архітектури, memory map, перекриттів, переповнення адрес, kernel entry, initrd і framebuffer.
- Host-тести валідатора та базовий make test.
- JSON Schema для boot manifest і каталог можливостей дев'яти доступних репозиторіїв.
- Архітектурна основа ОС, матриця цільових процесорів, security/recovery policy та етапи готовності.
- Контракти інтеграції Kloz, CO, SaintSI, ReaperCripple, SAMBU, Klipot, ToolNG і NUNLauncher.

## Статус

<strong>EARLY FOUNDATION · NOT BOOTABLE</strong>

У репозиторії ще немає firmware loader, linker script, kernel entry point, interrupt controller, scheduler, virtual memory manager, filesystem, драйверного стека або bootable image. Host-тести підтверджують лише перевірку boot metadata — вони не доводять завантаження на емуляторі чи фізичному обладнанні.

## Архітектура

- **NUN** володіє ядром, ABI, пам'яттю, процесами, IPC, безпекою та системними сервісами.
- **Kloz** надає вузьку переносну платформну основу; інтеграція вимагає аудиту та target-перевірки.
- **CO** і **SaintSI** можуть бути адаптованими сервісними компонентами, але не підміняють scheduler, watchdog чи криптографію.
- **ReaperCripple** постачає точні BSP/board-профілі та контрольовані bootstrap-процедури.
- **SAMBU** надає експериментальне мовне середовище й інструменти; native compiler ще не готовий для production.
- **Klipot** лишається незалежною контрольною площиною; **ToolNG** — Windows-діагностикою для робочої станції.

## Мови системної платформи

Fortran, Pascal та Delphi/Object Pascal плануються як повноцінні мови користувацького простору через версіонований C ABI, системні модулі, runtime-адаптери та бібліотеки. Fortran використовує ISO_C_BINDING/BIND(C); Pascal — FPC і C-compatible records; Delphi — окрему source/runtime compatibility matrix. Приватні runtime-об'єкти не перетинають межу ABI. SAMBU зберігає власний експериментальний toolchain.

## Цільові платформи

Перший еталонний напрям — x86_64 UEFI у QEMU/OVMF. AArch64 і RISC-V — наступні порти. Elbrus/E2K залишається окремим архітектурним напрямом без підтвердженого boot evidence. Підтримка Apple/Intel/AMD визначається конкретною архітектурою, прошивкою та платою, а не лише назвою виробника CPU.

## Збірка та тести

Потрібні C11-компілятор і Make. Запуск: <code>make test</code>, очищення: <code>make clean</code>.

Тест перевіряє переносні програмні інваріанти. Він не замінює QEMU/OVMF boot test, linker map, serial trace або перевірку фізичної плати.

## Документація

- [OS Foundation](docs/OS_FOUNDATION.md)
- [Boot information ABI](include/nun/boot_info.h)
- [Boot manifest schema](spec/boot-info-v1.schema.json)
- [System capability catalog](spec/system-capability-catalog.json)
- [Capability composition](docs/architecture/capability-composition.md)

## Ліцензія

Файл LICENSE у NUN відсутній; ліцензію не оголошено. До розповсюдження або включення коду інших репозиторіїв потрібно окремо узгодити ліцензійні умови кожного компонента.

---

**NUN — створено з любов'ю до системного програмування, надійності та людей.**
