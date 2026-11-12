# NUN
**Мінімальна основа ядра ОС: ранній x86_64 bring-up.**

Низькорівневий kernel-проєкт із розділеними boot targets і перевірюваними межами готовності. Це ранній прототип, не завершена ОС.

## Можливості
- Versioned boot contract і handoff від [NUNLauncher](https://github.com/bboroski/NUNLauncher).
- CPU entry, privilege transitions, exceptions та interrupt vectors.
- Physical pages, early allocator, virtual addressing і memory protection.
- Execution model, synchronization, device boundaries та diagnostics.
- Окремі readiness gates для build, emulator і hardware qualification.

## Запуск і перевірка
```bash
bash scripts/build-qemu.sh
bash scripts/run-qemu.sh

# окремий Limine target
make iso
make test
make run
```

## Додатково
NUN — kernel без графічного інтерфейсу; замість фальшивих GUI-знімків використовуйте boot logs та результати QEMU workflow у CI.

## Межі
Multiboot2 і Limine targets залишаються окремими. QEMU smoke-test не доводить готовність фізичного обладнання. E2K userspace runtime не є bare-metal E2K kernel.

## Пов'язані проєкти
[NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) · [Hellduck](https://github.com/bboroski/Hellduck)

## Ліцензія
Ліцензію не оголошено.

---
**「海枯石烂心不变。」**
*«Навіть коли висохнуть моря й розсиплються скелі, серце не зміниться».*
— Традиційний китайський вислів