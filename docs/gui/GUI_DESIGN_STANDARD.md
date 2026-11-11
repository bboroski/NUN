# GUI / Visual System Standard

> Design contract for future host/developer tools. NUN is currently specification-only and is not a bootable operating system.

## Visual identity

| Token | Value | Use |
|---|---|---|
| Primary yellow | `#FFCC00` | Main action and selected item |
| Highlight yellow | `#FFE600` | Focus and progress |
| Black / graphite | `#000000` / `#1A1A1A` | Structure and panels |
| Error red | `#E53935` | Critical validation/boot error |
| Signal cyan | `#29B6F6` | Active validation/data flow |
| White | `#FFFFFF` | Text and labels |

Existing Duck design tokens and marks remain canonical where present. A palette migration must update all consumers and checks together.

## Future developer/operator workspace

A future tool may show a header with project mark, actual validation stage, measured progress and local time; a central specification/boot-contract canvas; contextual controls; and a pager only for multi-view workflows. `55 / ~~~ / 21:33` are wireframe placeholders, not kernel telemetry.

## Specification boundary

- This repository currently records system contracts; it does not provide a bootable kernel, GUI, boot path, drivers or validated CPU target.
- A design mockup is not implementation evidence. Any visual tool must be identified as a separate host application.
- Display boot stages, image validation, handoff fields, architecture/ABI and evidence only when sourced from actual tested tools.
- Unknown CPU/platform capabilities must remain unknown and fail closed; do not fabricate progress or compatibility.
- Long-running builds and emulator runs report real queued/running/succeeded/failed states and expose logs/artifacts.

## Accessibility and release gate

Provide semantic labels, keyboard-visible focus, readable diagnostics, reduced-motion behavior and WCAG AA contrast targets (4.5:1 normal text; 3:1 large text). Label concept screens clearly. Before claiming an implemented GUI, provide a runnable host tool, tests, screenshots and documented platform boundaries. OS boot claims separately require build, linker map, emulator and target evidence.

## Українська

NUN наразі є специфікацією, а не завантажуваною ОС і не GUI. Майбутній інструмент розробника може показувати етапи валідації, boot contract, handoff та реальні результати збірки/емулятора. Значення `55 / ~~~ / 21:33` — лише макет. Не можна заявляти підтримку CPU, boot або драйверів без доказів. Жовтий `#FFCC00` — акцент, `#FFE600` — фокус, червоний `#E53935` — помилки, блакитний `#29B6F6` — активність.

## Русский

NUN сейчас является спецификацией, а не загружаемой ОС и не GUI. Будущий инструмент разработчика может показывать этапы валидации, boot contract, handoff и реальные результаты сборки/эмулятора. Значения `55 / ~~~ / 21:33` — только макет. Нельзя заявлять поддержку CPU, boot или драйверов без доказательств. Жёлтый `#FFCC00` — акцент, `#FFE600` — фокус, красный `#E53935` — ошибки, голубой `#29B6F6` — активность.
