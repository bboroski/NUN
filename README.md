# NUN — Minimal Kernel Substrate

**Репозиторій-специфікація для мінімального системного ядра.**

Поточний стан: **каркас документації**. У репозиторії є лише README; вихідного коду ядра, boot path, linker script, build system, драйверів і тестів ще немає. Цей документ фіксує технічний напрям і не видає заплановані компоненти за реалізовані.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md) · [Репозиторій](https://github.com/bboroski/NUN)

## Запропоновані межі системи

- **Boot contract:** формат handoff від завантажувача, карта пам'яті та версія ABI.
- **Architecture layer:** CPU entry, privilege transition, exception та interrupt vectors.
- **Memory layer:** фізичні сторінки, ранній allocator, віртуальна адресація й захист.
- **Execution layer:** task/context model і явна модель синхронізації.
- **Device boundary:** описані bus/driver interfaces без припущень про конкретну плату.
- **Diagnostics:** ранній serial/framebuffer output і структурований panic record.

## Інженерні вимоги

Кожна апаратно-залежна дія має бути прив'язана до конкретної архітектури й boot protocol. ABI, межі пам'яті, alignment, privilege level та interrupt policy мають бути задокументовані. Host-тести не можуть замінити емуляцію або перевірку на цільовій платформі.

## Поточна перевірка

Збірки та тестів поки немає: реалізація не створена. Перший кодовий етап має додати reproducible build, linker map, serial smoke test і автоматизовану перевірку образу.

## Технічна ідентичність

**NUN** — канонічне технічне ім'я мінімального kernel substrate. GitHub-шлях збережено лише як адресу репозиторію.

## Статус

Не використовувати як bootable OS і не завантажувати на фізичне обладнання: реалізованого ядра ще немає.
