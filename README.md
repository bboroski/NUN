# NUN — Minimal Kernel Substrate

**Репозиторій-специфікація мінімального системного ядра.**

Поточний стан: **архітектурна документація, без реалізації ядра**. Репозиторій містить README трьома мовами та специфікації; вихідного коду ядра, boot path, linker script, системи збірки, драйверів і тестів поки немає. Документи фіксують технічний напрям і не видають заплановані компоненти за реалізовані.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md) · [Репозиторій](https://github.com/bboroski/NUN)

## Запропоновані межі системи

- **Boot contract:** формат handoff від завантажувача, карта пам'яті та версія ABI.
- **Architecture layer:** CPU entry, перехід між рівнями привілеїв, exception та interrupt vectors.
- **Memory layer:** фізичні сторінки, ранній allocator, віртуальна адресація й захист.
- **Execution layer:** task/context model і явна модель синхронізації.
- **Device boundary:** описані bus/driver interfaces без припущень про конкретну плату.
- **Capability composition layer:** реєстр можливостей, перевірка сумісності, побудова композицій, ізольоване підключення та атомарне повернення до попереднього стану.

## Об'єднання та поглинання можливостей

NUN має отримати **Capability Composition & Assimilation** — системний механізм, який дозволяє приймати нові модулі, перевіряти їхні контракти та залежності, поєднувати сумісні можливості в складені можливості й повторно використовувати ці композиції як звичайні модулі.

Це не безконтрольне злиття виконуваного коду. Кожна можливість має версіонований маніфест, ідентифікатор, ABI, перелік інтерфейсів і мінімально необхідних дозволів. Підключення проходить через карантин, перевірку, розв'язання конфліктів, ізольоване тестове середовище та транзакційну активацію.

- [Архітектура Capability Composition & Assimilation](docs/architecture/capability-composition.md)
- [JSON Schema маніфесту можливості](spec/capability-manifest.schema.json)

## Інженерні вимоги

Кожна апаратно-залежна дія має бути прив'язана до конкретної архітектури й boot protocol. ABI, межі пам'яті, alignment, privilege level та interrupt policy мають бути задокументовані. Host-тести не можуть замінити емуляцію або перевірку на цільовій платформі.

Для системи композиції додатково обов'язкові: детерміноване розв'язання залежностей, відхилення циклічних залежностей, ізоляція модулів, принцип найменших привілеїв, походження артефактів, атомарна активація та перевірений rollback.

## Поточна перевірка

Збірки та тестів ядра поки немає: реалізація не створена. Специфікація композиції також є архітектурним контрактом, а не готовим runtime. Перший кодовий етап має додати відтворювану збірку, linker map, serial smoke test, перевірку образу та окремі host-тести реєстру й resolver.

## Технічна ідентичність

**NUN** — канонічне технічне ім'я мінімального kernel substrate. GitHub-шлях збережено лише як адресу репозиторію.

## Статус

Не використовувати як bootable OS і не завантажувати на фізичне обладнання: реалізованого ядра ще немає.


## UI foundation

The repository includes a standalone engineering desktop preview in `ui/`. Open `ui/index.html` in a modern browser. Its mineral, evergreen graphite, copper and jade design is intentionally distinct from the mobile-first NUNLauncher. The interface includes a system blueprint, project activity overview, component inventory and responsive engineering navigation.

The UI is a presentation prototype only. It does not implement the kernel or Capability Composition & Assimilation runtime; project graphs and statuses are illustrative metadata, not live telemetry.
