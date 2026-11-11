# Motion & GUI system — NUN

**Version:** 1.0  
**Adoption type:** repository-scoped presentation contract  
**Status:** design and engineering requirements; runtime capabilities must be verified independently.

## 1. Purpose and scope

This contract unifies animation, motion design, dynamic visualization, rendering, user-interface behavior and interaction feedback without turning independent products into one runtime. The vocabulary includes animation, movement, kinetic response, morphing, transformation, interpolation, rendering, simulation, deformation, frame-by-frame playback, micro-interactions, transitions, motion effects, dynamic graphics, computer animation, GUI, user interface, visual interface, application shell, control panel, workspace, screen environment, navigation and interaction surfaces.

The vocabulary describes possible presentation techniques and product surfaces; it is not a requirement to implement every technique in every binary. A repository adopts only capabilities supported by its actual product, platform and safety boundary.

## 2. Layering and invariants

Use this dependency direction:

`domain state → presentation model → interaction controller → renderer`

- Domain logic owns facts, lifecycle, evidence, device state and business outcomes.
- The presentation model maps facts to labels, values, accessible names and visual states.
- The interaction controller handles input and schedules optional visual changes.
- The renderer draws the current state; it must not infer domain truth from animation progress.
- Animation is presentation only. It must never trigger destructive actions, delay safety interlocks, alter deadlines, or imply success before the real result exists.
- Keep UI dependencies out of firmware, kernel, portable ABI, integrity, validation and other non-presentation cores.

## 3. Motion primitives and timing

Use motion only when it improves orientation, causality or feedback:

| Token | Default | Intended use |
|---|---:|---|
| `motion-none` | `0 ms` | Reduced motion, critical state changes, static fallback |
| `motion-fast` | `110 ms` | Press/release and focus feedback |
| `motion-standard` | `180 ms` | Hover, selection and small transitions |
| `motion-slow` | `240 ms` | Panel reveal and short view transition |
| `motion-max` | `320 ms` | Upper bound for ordinary UI transitions |

Use a monotonic clock for timed animation. Prefer opacity and transform over layout-affecting properties. Keep interpolation bounded and deterministic; cancel superseded transitions. Avoid spring overshoot, parallax, large zooms and perpetual motion by default. Longer explanatory sequences must be user-started, pausable and skippable.

Supported techniques include keyframes, interpolation/easing, transforms, compatible-state morphing, frame playback, progress visualization and simulation. Each needs a static equivalent and a clear relationship to underlying data.

## 4. Interaction and visual states

Define applicable states for every interactive component: idle, hover, keyboard focus, pressed, selected, pending, success, warning, error and disabled. Do not rely on hover alone. Pending is not success; unknown is not failure.

Prefer small cues: a visible focus ring, restrained press response, state-label change, short reveal or explicit progress indicator. Keep important information visible during motion. Progress must come from real work signals or be labelled indeterminate; never fabricate a percentage.

## 5. GUI and accessibility

- Preserve semantic controls, accessible names, keyboard operation, logical focus order and visible focus.
- Pair color with text or an icon for status, warning, selection and failure.
- Keep existing design tokens and native platform conventions; do not introduce a competing theme.
- Support resizing, text scaling, high contrast and platform-appropriate touch targets.
- Honor reduced-motion preferences. On the web, implement `prefers-reduced-motion: reduce`; on desktop, use the operating-system accessibility setting when available.
- Reduced-motion mode removes or instantly settles decorative transforms, parallax, morphing and autoplay while preserving information, navigation, focus, progress and task completion.
- Persistent or auto-updating motion must be avoidable and provide pause/stop controls when applicable.

## 6. Rendering and lifecycle

Rendering must be non-blocking and bounded. Do not run animation work on a critical worker, interrupt, device callback or real-time lifecycle path. Stop timers, animation frames and callbacks when a view is hidden, replaced or disposed. Avoid unnecessary redraws; pause work in background tabs/windows. Fall back to a static state if a renderer, GPU feature or animation API is unavailable.

For browser surfaces, prefer CSS transitions/keyframes for simple UI motion and reserve `requestAnimationFrame` for genuinely frame-driven visualizations. For native surfaces, use the platform event loop and accessibility APIs. Never add a browser engine, graphics library, scheduler or runtime dependency solely to satisfy this document.

## 7. Verification and acceptance

A UI or motion change is acceptable only when:
1. the same task works with motion disabled;
2. keyboard focus and keyboard-only operation remain correct;
3. the reduced-motion path is explicitly checked;
4. status labels and values reflect real application state;
5. callbacks are cancelled on navigation/teardown and do not leak;
6. scaling, narrow layouts and high-contrast behavior are checked where applicable;
7. relevant build/tests and documentation checks pass.

Prefer deterministic tests with a controllable clock or scheduler. Test state transitions and cancellation directly; avoid flaky tests tied to real frame timing. Visual previews are not proof of a running feature, and host tests are not hardware qualification.

## 8. Repository integration profile

NUN is an early x86_64 kernel bring-up and boot substrate, not a GUI operating system. Keep boot, interrupt, allocator, exception and serial-diagnostic paths deterministic and text-first. Do not add animation, graphical framebuffer or frame scheduling to kernel core as part of a presentation contract. A future graphical console must be a separately specified subsystem with explicit memory, display-driver and architecture readiness evidence.

## 9. Rollout boundary

This contract is introduced as an isolated, reviewable change. It does not claim that every listed animation type, GUI, renderer or client already exists. Implementations belong in the owning presentation layer and may be marked implemented only after runtime evidence exists. Existing APIs, ABI, formats, build targets, permissions and repository responsibilities remain unchanged.
