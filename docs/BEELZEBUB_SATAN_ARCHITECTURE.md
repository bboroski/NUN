# Beelzebub × Satan — архітектура інтеграції

> Статус: **архітектурна специфікація / proposal**. Документ не підтверджує реалізацію функцій.

## 1. Призначення

У цьому репозиторії застосовується двоконтурна модель:

- **Beelzebub — Assurance & Defense Plane:** перевірка цілісності, конфігурацій, provenance, тестів, доказів і відхилень; пояснення ризиків та рекомендації.
- **Satan — Coordination & Recovery Plane:** інвентаризація, міжрепозиторні контракти, аудит, звіти, планування контрольованих змін і release readiness.

Це архітектурні кодові назви, не обіцянка автономного інтелекту, безпомилкового захисту чи самовільного ремонту.

## 2. Історична точність

«Сатана» походить від семітського слова зі значенням «противник/обвинувач»; образ змінювався в біблійних текстах і пізнішій традиції. Ототожнення Сатани з Люцифером/Денницею є історією інтерпретацій, а не універсальною тотожністю всіх джерел.

Веельзевул пов’язаний із біблійною формою Баал-Зевув і давніми левантійськими культами. «Владика мух» — впливова полемічна інтерпретація; співвідношення форм Baal-zebub і Baal-zebul та їх етимологія мають наукові дискусії. «Князь демонів», міністр пекла та радник Сатани — ролі з різних шарів пізнішої демонології й літератури.

У технічній метафорі цього проєкту: **Beelzebub = виявлення деградації й assurance; Satan = координація, аудит і контрольоване відновлення.**

## 3. Локальний профіль NUN

Kernel/OS research and boot/build tooling. Beelzebub checks image manifests, build provenance, static invariants and emulator evidence; Satan coordinates kernel-related contract status. Host/QEMU success must not be presented as physical hardware qualification.

Інтеграція не змінює нативну мову, ABI, модель виконання, build-систему або платформні гарантії репозиторію.

## 4. Beelzebub: захисні модулі

1. **Preflight Inspector:** перевірка конфігурацій, залежностей, manifests та передумов.
2. **Build Evidence:** фіксація toolchain, параметрів, commit SHA, тестів і provenance артефактів.
3. **Integrity Monitor:** контроль очікуваних файлів, хешів і підписів, якщо підписання налаштовано.
4. **Policy & Threat Memory:** версіоновані правила та локальна історія відхилень без збирання секретів.
5. **Runtime/Target Guardian:** тільки перевірки, реально підтримані платформою; host-тести не замінюють target qualification.
6. **Recovery Advisor:** пропонує найменший виправний крок і докази, але не застосовує його самостійно.

## 5. Satan: координаційні модулі

1. **Repository Registry:** репозиторії, default branch, власники контрактів і статуси.
2. **Contract Graph:** версіоновані API, manifests, формати артефактів і сумісність.
3. **Audit Orchestrator:** запускає лише явно дозволені перевірки.
4. **Change Planner:** готує ізольовані зміни у feature branch і PR.
5. **Recovery Ledger:** журнал сигналу, доказів, рішення, автора та результату.
6. **Release Coordinator:** звіт про готовність без автоматичного merge/deploy.

## 6. Формат доказу

Кожен результат повинен містити `schema_version`, `repository`, `commit_sha`, `timestamp_utc`, `check_id`, `status`, `evidence_refs`, `tool_versions`, `policy_version`, `severity`, `explanation` та `recommended_action`.

Допустимі стани: `pass`, `fail`, `unknown`, `not_applicable`. Відсутність доказів — це `unknown`, а не автоматичний успіх або помилка.

## 7. Межі безпеки

- Read-only і least privilege за замовчуванням.
- Не читати, логувати або передавати секрети, приватні ключі, токени, особисті дані чи backup-вміст.
- Не виконувати код іншого репозиторію лише через його реєстрацію у федерації.
- Не змінювати `main`, release tags, production, firmware або hardware автоматично.
- Будь-який patch — окрема гілка, мінімальний diff, тести, audit trail і PR.
- P2P, підписи, SBOM, sandbox, threat intelligence та auto-healing є **planned**, доки реалізацію не підтверджено кодом і тестами.
- Не блокувати merge рекомендацією; обов’язкові gates визначаються явними branch protection rules.

## 8. План впровадження

**0 — Inventory:** реальні точки входу build/test, залежності, активи та власники контрактів.

**1 — Observe:** read-only перевірки, fixtures і структуровані звіти.

**2 — Verify:** schema validation, unit/integration tests, негативні сценарії та reproducibility.

**3 — Coordinate:** версіоновані контракти й агрегування результатів через Hellduck/Satan Nexus без виконання чужого коду.

**4 — Controlled remediation:** тільки після окремого схвалення — patch branch/PR; без автоматичного merge або deployment.

## 9. Definition of Done

- Визначені локальна зона відповідальності та trust boundary.
- Перевірки мають стабільні ID, версії та зрозумілі статуси.
- `unknown` не маскується як `pass`.
- Є позитивні, негативні й regression-тести.
- Секрети не потрапляють у звіти.
- Документація відповідає коду; planned не видається за implemented.
- Зміни проходять через PR, merge вирішує maintainer.

## 10. Принцип

**Beelzebub виявляє й пояснює деградацію. Satan пов’язує докази, контракти та план дій. Людина затверджує зміни.**
