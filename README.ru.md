# NUN
**Minimal Kernel Substrate**

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

Ранний x86_64 kernel bring-up и проект спецификаций. Это не полноценная операционная система.

## Возможности
- Версионированный boot handoff contract с NUNLauncher.
- Архитектурный слой: CPU entry, смена привилегий, exceptions и interrupt vectors.
- Требования к memory model, раннему allocator, виртуальной адресации и защите.
- Раздельные Multiboot2 и Limine boot targets с проверками в QEMU.

## Запуск и проверка
```bash
bash scripts/build-qemu.sh
bash scripts/run-qemu.sh
# Limine target
make iso
make test
```

## Границы
Multiboot2 и Limine targets независимы. QEMU smoke-тесты не квалифицируют физическое оборудование. E2K userspace не является bare-metal ядром E2K.

## Документация проекта
Для ядра наглядные доказательства — boot logs и результаты QEMU CI; GUI-скриншоты не заявляются.

## Связанные проекты
[Boot contract](docs/) · [NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby)

## Лицензия
Корневой LICENSE не объявлен; перед повторным использованием проверьте условия репозитория.

---
**海枯石烂心不变。**
*Даже если моря высохнут, а скалы разрушатся, сердце не изменится.*
— Традиционное китайское выражение
