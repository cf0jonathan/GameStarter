# Rocket Game Engine Demo

Component‑based 2D physics game implementing Box2D integration (see `assignment.txt`). Focus is strictly on features completed so far; no future roadmap included.

## Demo Overview
The current build launches a window where:
- A rocket sprite (player) rotates toward the mouse and applies thrust when left-click is held.
- The rocket has momentum and realistic physics (damping, rotation) via Box2D.
- The camera (View) smoothly follows the rocket.
- 50+ asteroids (static physics bodies) populate the scene with collision detection.
- Physics debug visualization (wireframes) can be toggled with `F1`.
- Movement and physics are frame‑rate independent via fixed timestep.

## Implemented Features

### Component Architecture
- **Core**: `GameObject` + `Component` base with polymorphic update/render.
- **Transform**: `TransformComponent` manages position and rotation, synced with physics.
- **Rendering**: `SpriteComponent` (textured quads), `BackgroundComponent` (parallax tiled stars).
- **Physics**: `PhysicsBodyComponent` wraps Box2D v3 bodies with dynamic/static types, circle/ellipse shapes.
- **Controls**: `InputComponent` (mouse/keyboard state), `MoveComponent` (thrust forces), `RotateToMouseComponent` (smooth angular acceleration).
- **Camera**: `CameraFollowComponent` sets `View` center; `View` transforms world→screen.

### Box2D Physics Integration (v3.x)
- **World**: Zero-gravity space environment (`b2WorldId`), fixed timestep simulation.
- **Bodies**: Dynamic (rocket with damping, rotation) and Static (asteroids).
- **Shapes**: Circle and Ellipse (approximated polygon), auto-sized from sprite dimensions.
- **Forces & Velocities**: `MoveComponent` applies thrust forces toward mouse; `RotateToMouseComponent` controls angular velocity with acceleration/deceleration.
- **Collision Detection**: Contact events enabled on all bodies; collisions detected between rocket and asteroids.
- **userData**: Box2D bodies store `GameObject*` pointers for event handling.
- **Debug Rendering**: `PhysicsDebugDraw` visualizes wireframes (toggle with `F1`).
- **Runtime Spawn**: Press `F2` to spawn new asteroids at mouse position (demonstrates add/remove at runtime).

### Asset & Configuration Management
- **XML-Driven**: `assets/config.xml` defines textures, object spawns, physics parameters, and component settings.
- **AssetManager**: Maps texture IDs to loaded SDL textures; provides fallback colored quads for missing assets.
- **Settings**: Display resolution and frame rates (render/logic) configurable in XML.

### Camera & Rendering
- **View System**: Transforms world coordinates to screen space; parallax background scrolling.
- **Frame Rate**: Configurable render FPS (120Hz) and fixed logic timestep (360Hz) for deterministic physics.
- **Layering**: Background rendered first, then sprites sorted by depth.

### Input
- **Mouse**: Left-click applies thrust; position drives rotation target; `F2` spawns asteroids.
- **Keyboard**: `F1` toggles physics debug overlay.
- **Component-Based**: `InputComponent` provides input state consumed by control components.

## Configuration (`assets/config.xml`)
Example structure:
```xml
<assets>
  <settings>
    <display width="1440" height="720" />
    <timing renderFPS="120" logicFPS="360" />
  </settings>
  <textures>
    <texture id="rocket" path="assets/rocket.png" />
    <texture id="asteroid" path="assets/asteroid.png" />
    <texture id="stars" path="assets/stars.png" />
  </textures>
  <gameObjects>
    <object type="background">
      <background texture="stars" tileWidth="800" tileHeight="600" scrollX="0.3" scrollY="0.3" />
    </object>
    <object type="player">
      <position x="0" y="0" />
      <sprite texture="rocket" width="120" />
      <physics type="dynamic" shape="ellipse" linearDamping="0.8" angularDamping="0.0" 
               fixedRotation="false" density="1.0" />
      <rotateToMouse rotationMaxSpeed="360" rotationAccel="720" rotationDecel="1080" 
                     rotationSlowRadius="60" rotationDeadZone="1" rotationEaseExponent="0.6" />
      <move thrustForce="2500.0" maxSpeed="4500.0" />
    </object>
    <object type="asteroid">
      <position x="-650" y="-420" />
      <sprite texture="asteroid" width="85" />
      <physics type="static" shape="circle" />
    </object>
    <!-- 49 more asteroids... -->
  </gameObjects>
</assets>
```
Modify positions, sizes, physics parameters, and add/remove objects without recompiling.

## Repository Structure (Key Parts)
- `src/` engine and component code (Box2D integration in `PhysicsBodyComponent`, `Engine`)
- `assets/config.xml` texture + spawn + physics definitions
- `.github/copilot-instructions.md` development guidance
- `gameGuidelines.txt` long‑term gameplay vision (mouse-following rocket in asteroid field)
- `assignment.txt` current academic requirements (Box2D integration)
- `CMakeLists.txt` build configuration with Box2D v3 dependency via vcpkg

## Assignment Mapping (Box2D Physics Integration)
| Assignment Requirement | Implemented In Code |
|------------------------|---------------------|
| Integrate Box2D with SDL game engine | `Engine` creates `b2WorldId`; `PhysicsBodyComponent` wraps `b2BodyId` |
| Use userData to store GameObject* | `b2BodyDef.userData` and `b2Body_SetUserData()` in `PhysicsBodyComponent::update()` |
| Apply force and set velocities | `MoveComponent` uses `b2Body_ApplyForceToCenter()`; `RotateToMouseComponent` uses `b2Body_SetAngularVelocity()` |
| Raycasting (optional) | *Not implemented (chose dynamic forces + contact listening instead)* |
| AABB Querying (optional) | *Not implemented (chose dynamic forces + contact listening instead)* |
| Contact Listening | Enabled via `b2Body_EnableContactEvents()` and `shapeDef.enableContactEvents` |
| Add/remove bodies at runtime | `F2` key spawns new asteroid; `PhysicsBodyComponent` destructor calls `b2DestroyBody()` |
| Static and Dynamic Bodies | Asteroids use `b2_staticBody`; rocket uses `b2_dynamicBody` |
| Demo with interactions | Rocket navigates asteroid field with realistic physics and collision |

All features above are demonstrated in the running demo.

## Controls
- **Left Mouse Button (hold)**: Apply forward thrust toward mouse position.
- **Mouse Position**: Rocket rotates to face mouse with smooth angular acceleration.
- **F1**: Toggle physics debug overlay (wireframe bodies).
- **F2**: Spawn new asteroid at mouse position (runtime body creation demo).

## Building & Running
```bash
# Configure with CMake preset (MinGW on Windows)
cmake --preset win-mingw-debug

# Build
cmake --build build/win-mingw-debug

# Copy assets
cmake --build build/win-mingw-debug --target copy_assets

# Run
./build/win-mingw-debug/demo.exe
```

## Technologies
- **SDL2**: Window, rendering, input.
- **SDL2_image**: PNG texture loading.
- **Box2D v3.x**: Physics simulation (bodies, shapes, forces, contacts).
- **tinyxml2**: XML parsing for configuration.
- **vcpkg**: Dependency management.
- **CMake**: Build system.