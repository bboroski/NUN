<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>NUN OS · Основа bare-metal операційної системи</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-FOUNDATION%20IN%20PROGRESS-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/BOOT-HOST%20CONTRACT%20TESTS-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**NUN — основа самостійної операційної системи: boot contract, архітектурні порти, пам’ять, виконання задач, драйвери, ізоляція та user space.**

## Архітектура продукту

NUN OS — окрема bare-metal ОС. NUNLauncher — самостійний Android Home Launcher, а не UEFI-завантажувач. Звичайний Android-додаток не може замінити bootloader пристрою або записувати захищені розділи прошивки.

Початкова ціль NUN OS — x86_64 + UEFI у QEMU/OVMF. AArch64 і RISC-V 64-bit заплановані як окремі порти, але поки не заявлені як реалізовані.

## Що вже закладено

- Версіонований C11 boot-information ABI v1.
- Перевірка magic, версії ABI, архітектури та зарезервованих прапорців.
- Валідація memory map: розмір, кількість, вирівнювання, типи, переповнення й перетини діапазонів.
- Перевірка діапазонів kernel image/initrd і узгодженості framebuffer-полів.
- Host-тести та GitHub Actions workflow для суворої C11-компіляції.
- Архітектура NUN OS, boot-chain, карта інтеграції екосистеми та мовні профілі Fortran/Pascal/Delphi.

Це **ще не bootable OS**: EFI loader, linker script, kernel entry, page tables, scheduler, драйвери, user space та готовий образ поки не реалізовані.

## Мовні підсистеми

Спільний системний ABI буде доступний через окремі binding/runtime-профілі:

- **Fortran:** модулі, числові типи, масиви, наукові обчислення та `ISO_C_BINDING`.
- **Pascal / Free Pascal:** units, records, sets, classes, generics, exceptions, RTTI та RTL-профілі.
- **Delphi:** Object Pascal, компоненти, властивості, події, RTTI та C ABI interoperability за наявності ліцензованого toolchain.

Компілятори й runtime — користувацькі підсистеми. Вони не завантажуються в привілейоване ядро.

## Екосистема

| Репозиторій | Системна роль |
|---|---|
| SAMBU | Мови, компілятори, IDE та developer tooling |
| ReaperCripple | BSP, архітектурні адаптери, bootstrap і rollback |
| Kloz | Arena memory, платформні контракти та resource budgets |
| CO | Lifecycle і bounded execution |
| SaintSI | Health observation, heartbeat і CRC-32 |
| ToolNG | Діагностика, evidence-модель і звіти |
| Klipot | Реєстр модулів, схеми, dependency graph |
| NUNLauncher | Android Home Launcher та користувацька оболонка |

Інтеграція виконується через версіоновані інтерфейси й адаптери. Прикладні та платформно-залежні функції не переносяться в kernel автоматично.

## Збірка й тести поточного host-контракту

```bash
make check
make test
```

Ці команди перевіряють лише boot-info validator. Вони не збирають EFI loader і не створюють завантажувальний образ.

## Документація

- [NUN OS foundation](docs/NUN_OS_FOUNDATION.md)
- [Boot chain and image contract](docs/BOOT_CHAIN_AND_IMAGE_CONTRACT.md)
- [Fortran, Pascal and Delphi integration](docs/LANGUAGE_RUNTIME_INTEGRATION.md)
- [All-repository capability map](docs/ECOSYSTEM_CAPABILITY_MAP.md)
- [Implementation roadmap](docs/IMPLEMENTATION_ROADMAP.md)
- [Capability composition](docs/architecture/capability-composition.md)

Підтримка архітектури, bootable-образу чи безпечного оновлення оголошується лише після реалізації та відповідних тестів в емуляторі й на конкретному цільовому пристрої.
