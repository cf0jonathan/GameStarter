# Copilot Project Instructions

## Guiding Documents
- Always anchor work to `gameGuidelines.txt` (the long‑term game vision).
- Always consider `assignment.txt` (current assignment requirements and constraints).
- When proposing changes, verify alignment with both files and call out any trade‑offs between short‑term assignment needs and long‑term game goals.

## Defaults and Tone
- Be concise, direct, and friendly. Prioritize actionable guidance.
- Prefer small, iterative changes that compile and run.
- When uncertain, ask one clarifying question and suggest a safe default.

## Implementation Priorities
- Maintain a component‑based architecture (GameObject + Components).
- Use XML (`assets/config.xml`) for asset and spawn configuration when possible.
- Physics: Require Box2D version 3.x only. Do not introduce legacy 2.x API patterns. Enforce with `find_package(box2d 3 CONFIG REQUIRED)` and reject suggestions using deprecated 2.x headers or constructs.

## Assets and Rendering
- Favor reusing textures and scaling at render time for variety.
- Keep fallback rendering (colored shapes) available for missing assets.

## Input
- Use a component‑based input approach (`InputComponent`) consumed by control components (e.g., `PlayerControlComponent`).

## Code Style
- C++17, minimal headers per file, forward‑declare where practical.
- Avoid unnecessary dependencies; keep `CMakeLists.txt` aligned with actual usage.
- Keep changes focused; avoid drive‑by refactors unrelated to the task.
- Prefer variables/constants over hardcoded magic numbers. Retrieve runtime values (window size, etc.) from appropriate systems rather than assuming fixed dimensions.
- When physics is added, prefer modern Box2D v3 creation patterns (explicit settings structs) and no backward‑compat shim layers.

## Validation
- After edits, let the user handle testing and demo verification.
- If adding features, include a minimal in‑engine demo to showcase them.

## Communication
- Summarize what changed, why, and how it maps to `assignment.txt` and `gameGuidelines.txt`.
- Offer next steps (tests, screenshots, README updates) when relevant.

