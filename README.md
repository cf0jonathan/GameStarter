# Rocket Game Engine Demo

Component‑based 2D demo implementing the current assignment requirements (see `assignment.txt`). Focus is strictly on features completed so far; no future roadmap included.

## Demo Overview
The current build launches a window where:
- A rocket sprite (player) appears and can move with WASD.
- The camera (View) smoothly follows the rocket.
- Multiple asteroids (static objects) are spawned via XML and demonstrate camera motion.
- Movement is frame‑rate independent using `deltaTime`.

## Implemented Features
- Component Architecture: `GameObject` + `TransformComponent`, `InputComponent`, `PlayerControlComponent`, `SpriteComponent`, `CameraFollowComponent`.
- Asset & Spawn Configuration: XML (`assets/config.xml`) drives texture IDs and object spawns.
- Texture Management: `AssetManager` maps texture IDs to loaded SDL textures; fallback is colored quad.
- View / Camera System: `View` tracks center; player‑following camera transforms world→screen for all sprites.
- Frame Rate Limiting: Target FPS + `deltaTime` for time‑based movement.
- Input: WASD captured per‑frame via `InputComponent`, applied through `PlayerControlComponent`.

## Configuration (`assets/config.xml`)
Example structure:
```xml
<assets>
  <textures>
    <texture id="rocket" path="assets/rocket.png" />
    <texture id="asteroid" path="assets/asteroid.png" />
  </textures>
  <gameObjects>
    <object type="player"> <position x="0" y="0" /> <sprite texture="rocket" width="80" height="40" /> </object>
    <object type="asteroid"> <position x="-300" y="-200" /> <sprite texture="asteroid" width="96" height="96" /> </object>
    <!-- Additional objects ... -->
  </gameObjects>
</assets>
```
Modify positions, sizes, and add/remove objects without recompiling.

## Repository Structure (Key Parts)
- `src/` engine and component code
- `assets/config.xml` texture + spawn definitions
- `.github/copilot-instructions.md` development guidance
- `gameGuidelines.txt` long‑term gameplay vision
- `assignment.txt` current academic requirements

## Assignment Mapping
| Assignment Requirement | Implemented In Code |
|------------------------|---------------------|
| Sprites appear on screen | `SpriteComponent` rendering rocket + asteroids |
| Keyboard input triggers actions | `InputComponent` + `PlayerControlComponent` (WASD movement) |
| View updates based on sprite location | `CameraFollowComponent` sets `View` center to player transform |
| Asset management via XML | `assets/config.xml` + `AssetManager` texture loading + spawn data |
| Retrieve textures from a map | `AssetManager` internal `std::map` id→texture |
| Modify drawing to account for view | `View::worldToScreen` used by `SpriteComponent::render()` |
| Frame rate limiting and deltaTime | Game loop in `Engine::run()` sets target FPS and updates `deltaTime` |
| Use deltaTime in movement | `TransformComponent::update()` applies velocity * deltaTime |

All features above are demonstrated in the running demo.