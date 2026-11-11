# NEON SYNDICATE — interface and visual specification

**Repository:** NUN  
**Project focus:** KERNEL CONTRACTS / BOOT / MEMORY / EXECUTION  
**Visual category:** KERNEL SPECIFICATION / ARCHITECTURE BOARD

## Identity and palette
Use an original cyber-noir language: high-contrast Void Black `#050505`, Warning Yellow `#FCEE0A`, System Red `#FF003C`, Data Blue `#00F0FF` and Interface White `#FFFFFF`. Carbon panels use `#0D1115` and `#11171C`; supporting text uses `#A6B2BC`. Do not copy protected game art, logos or interface assets.

Semantic color never stands alone: pair it with text or an icon. Red indicates critical errors; cyan is informational; yellow signals a primary action or attention.

## Typography and composition
Use Orbitron/Rajdhani for uppercase display headings, Rajdhani/system sans-serif for body text and Roboto Mono/Share Tech Mono for IDs, code and measurements. Prefer asymmetrical grids, fine structural lines, chamfered panels, subtle hex textures and tabular numerals. Supply local font fallbacks.

## Motion and accessibility
Use restrained edge glows and short 120–260 ms transitions. Glitch effects must not obscure content or focus. Respect `prefers-reduced-motion`. Maintain keyboard focus, controls of at least 40 px and 44 px touch targets. Keep body copy readable and never rely on color alone.

## Project-specific focus
KERNEL CONTRACTS / BOOT / MEMORY / EXECUTION

- BOOT CONTRACT
- MEMORY MODEL
- EXECUTION MODEL
- Reference flow: ARCHITECTURE → MEMORY → SCHEDULING → DEVICES

The vector is a project-specific architecture/presentation concept; it does not assert a GUI or implemented features beyond the repository's documented scope.

## Screenshot policy
Runtime screenshots must be captured from an actual running application at a stated viewport. Prefer 1920×1080 or higher for desktop. Use PNG/WebP for real captures, label synthetic data, remove private paths and credentials, and never label an SVG mockup as a screenshot.

## Assets
- `assets/neon-syndicate/overview.svg` — 1920×1200 vector preview.
- `assets/neon-syndicate/theme.css` — shared web theme layer.
- `design/neon-syndicate.tokens.json` — machine-readable tokens.
