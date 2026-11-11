<!-- Title: NUN — minimal kernel substrate and C11 system integration -->
<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Логотип NUN" width="76"></p>

# NUN

> **Visual identity:** Duck Black / Yellow — accent `#FFD600`, primary ink `#111111`.

[![CI](https://github.com/bboroski/NUN/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/bboroski/NUN/actions/workflows/ci.yml)

**NUN** — системний проєкт, що визначає boot, architecture, memory та execution contracts і збирає host-side C11 integration/runtime slice із пов'язаних модулів.

<sub>Оновлено: 2026-11-11 · Версія CMake: 0.1.0 · Стадія: development / not bootable</sub>

> **TL;DR:** клонуйте репозиторій разом із submodules, зберіть CMake-проєкт і запустіть CTest. Цей host build перевіряє інтеграцію модулів, але **не створює завантажувальну операційну систему**.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

## Зміст

- [Призначення](#призначення)
- [Поточний обсяг](#поточний-обсяг)
- [Вимоги та швидкий старт](#вимоги-та-швидкий-старт)
- [Архітектурна композиція](#архітектурна-композиція)
- [Перевірка та межі готовності](#перевірка-та-межі-готовності)
- [Безпека](#безпека)
- [Документація та пов'язані репозиторії](#документація-та-повязані-репозиторії)
- [FAQ](#faq)
- [Підтримка та ліцензія](#підтримка-та-ліцензія)

## Призначення

NUN формує системні контракти майбутнього мінімального ядра: boot handoff, архітектурний entry, пам'ять, execution, synchronization та межі пристроїв. Поточний репозиторій також містить CMake-композицію host-side runtime і тестів.

## Поточний обсяг

- Versioned boot handoff contract, пов'язаний із [NUNLauncher](https://github.com/bboroski/NUNLauncher).
- C11 runtime target і host tests.
- Інтеграція source modules для ReaperCripple, ToolNG, Kloz, CO та SaintSI.
- Документовані capability gates для майбутнього cross-build, emulator і target qualification.

**Статус:** NUN наразі не є завантажувальною ОС. Наявність runtime library та CMake build не доводить, що реалізовані повний kernel boot path, privilege transitions або драйверний стек.

## Вимоги та швидкий старт

Потрібні Git із підтримкою submodules, CMake **3.20+** і C-компілятор із підтримкою C11.

~~~bash
git clone --recurse-submodules https://github.com/bboroski/NUN.git
cd NUN
cmake -S . -B build -DNUN_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
~~~

Якщо репозиторій уже клоновано без вкладених модулів, ініціалізуйте їх командою:

~~~bash
git submodule update --init --recursive
~~~

## Архітектурна композиція

Root CMake project додає модулі через pinned source repositories і збирає NUN.Runtime разом із тестовими targets. Базові компоненти зберігають власні контракти та каталоги; спільна збірка не скасовує їхню автономність.

Почніть із [capability composition](docs/architecture/capability-composition.md), [ecosystem integration](docs/ECOSYSTEM_INTEGRATION.md) та [technical debt register](docs/TECHNICAL_DEBT_REGISTER.md). Код runtime: [src](src) · [include](include) · тести: [tests](tests).

## Перевірка та межі готовності

CTest перевіряє host runtime та низку інтегрованих модулів. Успішний host build не є доказом boot на фізичному CPU, емуляторі або платі. Перед оголошенням target support потрібні конкретні toolchain, linker/startup files, image format, emulator evidence і hardware qualification.

[CI workflow](.github/workflows/ci.yml) запускає наявні автоматизовані перевірки. Відкриті PR або specification documents не вважаються реалізованими функціями.

## Безпека

Не використовуйте неперевірені адреси пам'яті, register maps, interrupt vectors або boot parameters на фізичному обладнанні. Межі пам'яті та privilege transitions мають підтверджуватися для конкретної архітектури й ревізії платформи.

**Ліцензія не оголошена:** у репозиторії немає файлу LICENSE. Не припускайте дозволу на зовнішнє розповсюдження.

## Документація та пов'язані репозиторії

- [NUNLauncher](https://github.com/bboroski/NUNLauncher) — окремий host validator boot-image contract.
- [Kloz](https://github.com/bboroski/Kloz) — portable platform substrate; [CO](https://github.com/bboroski/CO) — lifecycle runtime.
- [SaintSI](https://github.com/bboroski/SaintSI) — runtime health observations.
- [ReaperCripple](https://github.com/bboroski/ReaperCripple) — embedded BSP та manufacturing layer.
- [ToolNG](https://github.com/bboroski/ToolNG) — Windows device diagnostic application.

## FAQ

**Чи можна завантажити NUN як ОС?** Ні. У репозиторії немає підтвердженого bootable OS image і завершеного boot path.

**Навіщо потрібні submodules?** Вони фіксують версії окремих модулів, які включає root CMake build.

**Чи підтверджує CTest підтримку конкретного CPU?** Ні. Для цього потрібні cross-build та окремі emulator/target докази.

## Підтримка та ліцензія

Проблеми: [Issue tracker](https://github.com/bboroski/NUN/issues) · [bug report](https://github.com/bboroski/NUN/issues/new). PR має посилатися на пов'язану задачу, містити тести й пояснювати вплив на системний контракт. Профіль супроводу: [bboroski](https://github.com/bboroski).

**Ліцензія не оголошена:** файл LICENSE відсутній. Умови повторного використання мають бути визначені правовласником.
