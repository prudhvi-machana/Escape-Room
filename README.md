# Escape Room Project README

## 1. Project Overview

This project is a small first-person 3D escape room game built in **C++ using OpenGL and GLUT**.  
The player explores a single room, observes clues, interacts with objects, solves a puzzle, finds a key, and finally opens the door to escape.

From an interview or viva point of view, this project demonstrates:

- Real-time 3D rendering with OpenGL fixed-function pipeline
- Scene organization using modular C++ files
- Camera movement and mouse-look controls
- Item interaction and game-state management
- Simple puzzle design and progression logic
- Use of procedural textures and external PPM textures
- 2D HUD and overlay rendering on top of a 3D scene

This is not a game engine. It is a **scene-driven OpenGL application** where rendering, interaction, puzzle state, and object logic are manually organized into modules.

## 2. Core Gameplay Flow

The intended gameplay flow is:

1. The player starts inside a locked room.
2. The room contains multiple interactable items:
   - Door
   - Light switch
   - Desk drawer
   - Drawer book
   - Code box / chest
   - Brass key
   - Highlighted book
   - Dustbin paper
3. The player can inspect clue-bearing objects:
   - A highlighted book gives an observation hint.
   - A paper in the dustbin hints that a book is out of place.
   - The drawer book overlay visually hides the letters **K, E, Y** in its text.
4. The code box accepts a 3-letter input.
5. Entering `KEY` unlocks the chest.
6. The chest lid animates open and reveals a brass key.
7. The player picks up the brass key.
8. The player interacts with the door.
9. The door opens and the player can move outside the room.

That complete flow is implemented through shared game-state variables and interaction checks rather than a formal state machine class.

## 3. Architecture Summary

The project is divided into these functional areas:

- `main.cpp`: application startup and OpenGL/GLUT initialization
- `app/`: frame rendering and viewport handling
- `camera/`: player movement, mouse look, collision limits
- `room/`: room shell, skybox, fixtures, outdoor scene, chest, door
- `objects/`: furniture and decorative/interactive object models
- `utils/`: shared state, textures, interactions, HUD, helper declarations
- `resources/`: external texture images used by objects and the room

The public drawing loop is roughly:

1. GLUT calls `display()`
2. `display()` updates HUD animation and timing
3. Camera view is computed
4. Lights are configured
5. Room and props are drawn
6. HUD or overlays are drawn in screen space
7. Frame is swapped

## 4. Rendering Approach

The rendering uses the classic OpenGL fixed-function pipeline:

- `glBegin/glEnd` primitives
- `glMaterial`, `glLight`, `glTexCoord`, `glNormal`
- `gluPerspective`, `gluLookAt`
- GLUT callbacks for input and the main loop

This means the project does not use:

- shaders
- VBOs/VAOs
- modern OpenGL pipeline
- external game engines

That is important in a viva because you can explain that the focus here is on **scene composition, interaction logic, and modular game structure**, not on modern GPU abstraction layers.

## 5. Controls

- `W/A/S/D`: move
- `Mouse`: look around
- `Space`: move up
- `C`: move down
- `E`: interact with nearby object
- `Tab`: toggle mouse capture
- `Left Click`: interact when mouse is captured
- `Esc`: exit or close the code-box overlay
- Arrow keys: alternate camera look controls
- Mouse wheel: vertical movement

## 6. Important Design Ideas to Explain in Viva

### 6.1 Scene composition

The entire room is manually modeled using helper functions like `drawQuad()`, `drawBox()`, and `drawFloorShadowAABB()`.  
Each prop is drawn in its own file, which makes the project easier to maintain and explain.

### 6.2 Shared game state

Global shared variables store gameplay state:

- whether the room light is on
- whether the drawer is open
- whether the chest is unlocked
- chest opening progress
- inventory contents
- HUD message text
- active overlays

This is simple but effective for a compact academic project.

### 6.3 Interaction system

The player does not click exact mesh triangles.  
Instead, each interactable object has:

- a name
- a world position
- an interaction radius
- an action string
- descriptive text

The code selects the nearest / best-facing item and triggers behavior based on its name and action.

### 6.4 Puzzle logic

Puzzle progression is implemented with direct checks:

- clues are embedded in books and paper
- `KEY` unlocks the chest
- unlocked chest reveals key after animation progress
- key allows door opening

### 6.5 HUD and overlays

The 3D scene is drawn first, then the projection switches to 2D orthographic mode to render:

- crosshair
- prompts
- inventory
- message banners
- full-screen clue/code overlays

### 6.6 Texture strategy

The project uses two texture approaches:

- procedural textures for general materials like wood, plaster, brass, fabric
- external PPM textures for specific assets like floor, dustbin, painting, and skybox

That combination keeps the project lightweight while still making it visually richer.

## 7. File-by-File Explanation

Below is a detailed explanation of every relevant file in the repository.

---

## 7.1 Root Files

### `main.cpp`

Purpose:

- Entry point of the application
- Initializes GLUT window and OpenGL state
- Calls startup functions
- Registers all GLUT callbacks

Key responsibilities:

- Creates the game window titled `"Escape Room"`
- Enables:
  - depth testing
  - lighting
  - two lights (`GL_LIGHT0`, `GL_LIGHT1`)
  - color material
  - normalization
  - blending
- Sets global ambient scene light and clear color
- Calls:
  - `initTextures()` to prepare procedural textures
  - `initRoom()` to initialize room state and interactable items
- Connects input/render callbacks:
  - `display`
  - `reshape`
  - `keyboard`
  - `specialKeys`
  - `mouseButton`
  - `mouseMotion`
- Starts the GLUT main loop

Interview explanation:

This file is the application bootstrapper. It does not contain gameplay logic; it wires the engine-style lifecycle together.

### `makefile`

Purpose:

- Builds the full C++ project into the executable `escape_room`

Key responsibilities:

- Defines compiler: `g++`
- Defines warning flag: `-Wall`
- Links OpenGL libraries:
  - `-lGL`
  - `-lGLU`
  - `-lglut`
- Lists all source files in `SRCS`
- Provides:
  - `all` target to build
  - `clean` target to remove the executable

Interview explanation:

This is a simple manual build file, useful for showing that the project is modularized into multiple translation units rather than one monolithic file.

### `package.json`

Purpose:

- Currently unused placeholder file

Details:

- Contains `{}` only
- Does not affect the C++ build

### `package-lock.json`

Purpose:

- Present in the repository but not functionally used by the C++ OpenGL project

Interview explanation:

You can say these Node-related files are incidental and not part of the game runtime.

---

## 7.2 App Layer

### `app/render.h`

Purpose:

- Declares the main rendering functions used by GLUT

Functions:

- `display()`: renders one frame
- `reshape(int w, int h)`: updates projection when window size changes

### `app/render.cpp`

Purpose:

- Implements the per-frame render pipeline

Key responsibilities:

- Keeps frame timing with `gLastTime`
- Updates HUD animation by computing `delta` time
- Clears the frame and depth buffer
- Computes camera forward direction
- Sets the camera view using `gluLookAt`
- Configures scene lighting in `setupLights()`
- Draws all scene parts in order:
  - room shell
  - door
  - table
  - chair
  - dustbin
  - bookshelf
  - bed
  - painting
- Draws HUD and overlays
- Swaps display buffers

Lighting details:

- `GL_LIGHT0`: ceiling room light
  - brightness depends on `roomLightOn`
- `GL_LIGHT1`: desk lamp spotlight
  - always placed over the desk
  - warmer focused light

Interview explanation:

This file is the scene orchestrator. It does not build geometry itself; instead, it coordinates timing, camera, lighting, and the draw order for all modules.

---

## 7.3 Camera and Input

### `camera/camera.h`

Purpose:

- Declares camera position/orientation globals and input functions

Key data:

- `camX`, `camY`, `camZ`: camera position
- `angleY`, `angleX`: yaw and pitch
- `winW`, `winH`: window size
- `warping`, `mouseCaptured`: mouse control flags

Functions:

- keyboard and special-key handlers
- mouse motion and button handlers
- direction helpers:
  - `forward3D`
  - `rightXZ`
- `clampPitch`

### `camera/camera.cpp`

Purpose:

- Implements first-person movement and look controls

Key responsibilities:

- Stores movement constants:
  - movement speed
  - look sensitivity
  - pitch clamp
- Maintains initial camera position
- Handles:
  - keyboard movement
  - vertical movement
  - mouse look
  - interaction trigger
  - mouse capture toggle
- Restricts player movement with room boundaries
- Prevents walking through the chest until it is sufficiently open
- Allows leaving the room only after the door opens

Important helper functions:

- `pushOutOfAABB(...)`
  - simple collision against an axis-aligned bounding box
  - used for chest collision
- `clampToRoom(...)`
  - applies movement limits depending on whether the door is open
- `moveCamera(...)`
  - computes next position, clamps it, applies collision
- `forwardXZ(...)`, `forward3D(...)`, `rightXZ(...)`
  - generate movement/view vectors from camera angles

Interview explanation:

This file acts like a small first-person controller. It combines input processing, camera orientation math, collision restrictions, and puzzle-aware movement rules.

---

## 7.4 Shared Utility Layer

### `utils/utils.h`

Purpose:

- Central shared header for the whole project

Key contents:

- `TextureId` enum for all built-in procedural textures
- `Item` struct for interactable world items
- external declarations for shared global state
- declarations for drawing helpers
- declarations for interaction helpers
- declarations for HUD functions

Why it matters:

This header is effectively the common interface used by most modules.

### `utils/state.cpp`

Purpose:

- Defines all shared global game-state variables declared in `utils.h`

State stored here:

- `worldItems`
- `inventory`
- `hudMessage`
- `hudMessageTimer`
- `roomLightOn`
- `drawerOpen`
- `drawerBookOverlayActive`
- `codeBoxOverlayActive`
- `codeBoxUnlocked`
- `codeBoxOpenProgress`
- `codeBoxInput`

Interview explanation:

This file is the central storage for runtime game state. The rest of the project reads and modifies these values.

### `utils/lighting.h`

Purpose:

- Stores fixed light positions as named constants

Defined lights:

- ceiling light position
- desk lamp position

Interview explanation:

Separating light positions into constants makes the render code cleaner and avoids duplicated magic numbers.

### `utils/drawing.cpp`

Purpose:

- Implements material setup, procedural texture generation, texture loading, and textured quad drawing

This is one of the most important files technically.

Main responsibilities:

- Creates built-in procedural textures for materials like:
  - plaster
  - floor
  - wood
  - dark wood
  - metal
  - fabric
  - paper
  - plastic
  - book cover
  - wallpaper
  - grass
  - brass
- Loads external PPM textures from disk and caches them
- Computes quad normals and texture coordinate scaling
- Draws:
  - `drawQuad()`
  - `drawExternalTexturedQuad()`
  - `drawExternalImageQuad()`
- Applies OpenGL material values with `applyMaterial()`

Important technical ideas:

- `hashNoise()` provides pseudo-random variation
- `fillTexture()` uses noise and math patterns to synthesize believable material surfaces
- `readPpmHeader()` parses PPM image headers
- `gExternalTextures` caches already loaded textures so files are not repeatedly reloaded
- `computeQuadFrame()` computes normals and texture repetition from geometry itself

Interview explanation:

This file shows strong graphics understanding because it handles both procedural asset generation and traditional image-based texturing while keeping the rest of the code simple.

### `utils/interactions.cpp`

Purpose:

- Implements all item interaction logic and puzzle rules

Key responsibilities:

- Determines which item is near and most directly in front of the player
- Handles item-specific behavior
- Manages overlays for the drawer book and code box
- Checks inventory and puzzle conditions

Important functions:

- `getNearbyItem(...)`
  - finds the best target based on distance and viewing direction
- `getItemByName(...)`
  - searches the item list by string name
- `hasInInventory(...)`
  - checks if player owns an item
- `interactWithNearbyItem(...)`
  - main gameplay interaction dispatcher

Notable gameplay logic:

- light switch toggles `roomLightOn`
- drawer can open/close
- drawer book opens a clue overlay
- code box opens input overlay unless already unlocked
- correct code is `"KEY"`
- brass key can only be collected after chest opening progress reveals it
- door opens only if brass key is in inventory

Interview explanation:

This is the main gameplay logic file. It acts like a lightweight interaction controller and puzzle manager.

### `utils/hud.cpp`

Purpose:

- Draws all 2D interface elements and animated overlays

Key responsibilities:

- Renders:
  - crosshair
  - interaction prompts
  - temporary HUD messages
  - inventory list
  - drawer-book fullscreen overlay
  - code-box fullscreen overlay
- Updates timer-driven HUD values
- Animates `codeBoxOpenProgress` over time

Important ideas:

- switches from 3D projection to 2D orthographic projection
- disables lighting/depth during overlay drawing
- restores OpenGL state afterward

Drawer book overlay:

- intentionally stylized text
- letters `K`, `E`, and `Y` are highlighted in red
- acts as the hidden code clue

Code box overlay:

- shows user-entered letters
- displays usage instructions
- communicates chest status

Interview explanation:

This file is a good example of combining 3D gameplay with a 2D user interface in OpenGL.

---

## 7.5 Room System

### `room/room.h`

Purpose:

- Public interface for room module

Functions:

- `initRoom()`
- `drawRoom()`
- `drawDoor()`

### `room/room_internal.h`

Purpose:

- Internal shared header used only by room implementation files

Contents:

- geometric constants for:
  - chest dimensions
  - room dimensions
  - front wall
  - door frame
- skybox face enum
- internal shared texture/state variables
- declarations for split room helper functions

Interview explanation:

This file exists to keep the room code modular after splitting the original large room source into smaller files.

### `room/room.cpp`

Purpose:

- Contains the public room API and room initialization logic

Key responsibilities:

- Initializes skybox textures and floor texture
- Resets puzzle state
- Populates `worldItems` with all interactable objects
- Draws:
  - room floor
  - ceiling
  - walls
  - front wall split around the door opening
  - trim
  - ceiling light fixture
  - light switch
  - code box
- Draws the animated door and frame

Important design choice:

Interactable objects are added here as `Item` instances, so this file defines the world’s gameplay-relevant object list.

### `room/room_skybox.cpp`

Purpose:

- Handles skybox texture loading and outdoor environment drawing

Key responsibilities:

- Loads six PPM textures for the skybox
- Stores skybox texture IDs
- Draws skybox cube faces around the scene
- Draws outdoor ground layers and path beyond the room

Why it matters:

Once the door opens, the player can see and enter the outdoor area, so this file creates the sense that the room exists inside a larger world.

### `room/room_features.cpp`

Purpose:

- Draws room-specific fixtures and decorative architectural features

Key responsibilities:

- Computes and draws projected shadow for the door
- Draws ceiling light fixture
- Draws wall light switch
- Draws trim and crown molding around the room

Interview explanation:

This file focuses on environment polish. It adds realism without mixing this logic into the main room shell code.

### `room/room_codebox.cpp`

Purpose:

- Draws the chest / code box puzzle object and the brass key model

Key responsibilities:

- Builds the chest body, inner cavity, lid, hinges, latch, and indicator
- Animates lid opening based on `codeBoxOpenProgress`
- Draws brass key only after the chest is sufficiently open
- Adds bobbing motion to the revealed key

Important puzzle visualization:

- chest indicator changes color depending on lock state
- chest opening is animated smoothly
- key reveal is delayed until enough opening progress is reached

Interview explanation:

This file is a good showcase of combining geometry modeling, animation easing, and puzzle feedback in one object-specific module.

---

## 7.6 Object Utilities

### `objects/object_utils.h`

Purpose:

- Declares reusable 3D object helper functions

Functions:

- `drawBox(...)`
- `drawExternalTexturedBox(...)`
- `drawFloorShadowAABB(...)`

### `objects/object_utils.cpp`

Purpose:

- Implements reusable geometry and shadow helpers for all object files

Key responsibilities:

- Builds boxes by combining six quads
- Builds externally textured boxes similarly
- Projects soft floor shadows from light positions using bounding boxes
- Adds extra desk-lamp shadow contribution for objects near the desk

Interview explanation:

This file avoids repeated geometry logic across every furniture file and is important for code reuse.

---

## 7.7 Object Modules

### `objects/objects.h`

Purpose:

- Public header declaring all room object draw functions

Functions:

- `drawTable()`
- `drawChair()`
- `drawDustbin()`
- `drawBookshelf()`
- `drawBed()`
- `drawPainting()`

### `objects/table.cpp`

Purpose:

- Draws the desk/table, desk drawer, drawer book, and desk lamp

Key responsibilities:

- Draws table surface and legs
- Draws drawer housing and front panel
- Slides drawer outward when `drawerOpen` is true
- Draws the hidden drawer book only when drawer is open
- Draws a desk lamp model

Gameplay relevance:

- the drawer book is part of the puzzle clue chain

### `objects/chair.cpp`

Purpose:

- Draws the chair beside the table

Key responsibilities:

- Draws seat, legs, back support, and upholstery
- Adds a floor shadow

Interview explanation:

This file is mainly scene dressing and shows how furniture is manually modeled with boxes.

### `objects/dustbin.cpp`

Purpose:

- Draws the dustbin/trash can and the paper clue inside it

Key responsibilities:

- Models a cylindrical dustbin using segmented quads
- Supports an external dustbin texture
- Draws:
  - bin outer shell
  - liner interior
  - top rim
  - pedal
  - loose papers
  - the important clue paper

Technical interest:

- uses circular geometry via trigonometry
- mixes custom cylinder logic with standard box helpers

Gameplay relevance:

- contains one of the main clue objects in the room

### `objects/bookshelf.cpp`

Purpose:

- Draws the bookshelf and rows of books

Key responsibilities:

- Defines book specifications through structured data
- Calculates row widths dynamically
- Builds bookshelf frame and shelves
- Places books in bottom, middle, and top rows

Interesting design point:

- the shelf rows are data-driven through `ShelfBookSpec`, which is cleaner than hardcoding every single book separately

Gameplay relevance:

- visually supports the clue about books belonging on shelves

### `objects/bed.cpp`

Purpose:

- Draws the bed and bedding

Key responsibilities:

- models frame, mattress, blanket, and pillows
- adds floor shadow

Interview explanation:

This is another environment object that helps make the room feel complete and believable.

### `objects/painting.cpp`

Purpose:

- Draws the wall painting on the back wall

Key responsibilities:

- draws wooden frame
- loads external painting texture from `resources/painting/1.ppm`
- preserves image aspect ratio while fitting inside frame
- falls back to a procedural textured quad if external image fails

Interview explanation:

This file shows robust rendering because it includes a graceful fallback path if the texture is unavailable.

---

## 7.8 Resource Files

These files provide external textures used by the scene.

### `resources/Mantel.ppm`

- Used as the room floor texture in `room/room.cpp`

### `resources/Mantel.jpg`

- Alternate image version of the floor texture, not the primary runtime-loaded file

### `resources/dustbin.ppm`

- Loaded by `objects/dustbin.cpp` for the dustbin body and pedal

### `resources/dustbin.jpg`

- Alternate image version of the dustbin texture

### `resources/darkwood.ppm`

- Present as a texture asset but not actively used by the current runtime code after procedural/material refactoring

### `resources/darkwood.jpg`

- Alternate image version of the dark wood asset

### `resources/book1984.jpg`

- Present in repository, likely related to content iteration or planned/older asset use

### `resources/painting/1.ppm`

- Main runtime texture for the wall painting

### `resources/painting/1.jpg`

- Alternate image version of the painting

### Skybox textures

These six PPM files are loaded by `room/room_skybox.cpp`:

- `resources/skybox/front.ppm`
- `resources/skybox/back.ppm`
- `resources/skybox/left.ppm`
- `resources/skybox/right.ppm`
- `resources/skybox/top.ppm`
- `resources/skybox/bottom.ppm`

The matching `.jpg` versions are also present:

- `resources/skybox/front.jpg`
- `resources/skybox/back.jpg`
- `resources/skybox/left.jpg`
- `resources/skybox/right.jpg`
- `resources/skybox/top.jpg`
- `resources/skybox/bottom.jpg`

Interview explanation:

PPM files are easy to parse manually in C++, which is why they are convenient for teaching/demo projects.

## 8. How the Puzzle is Technically Implemented

### Step 1: Interactable registration

`initRoom()` creates all important interactable items and stores them in `worldItems`.

### Step 2: Item detection

`getNearbyItem()` checks:

- distance to each item
- whether the item is currently visible/available
- whether the player is facing it

### Step 3: Action dispatch

`interactWithNearbyItem()` decides what happens based on the item name.

### Step 4: Clue chain

- highlighted book gives observation hint
- dustbin paper suggests a book clue
- drawer book overlay reveals `KEY`

### Step 5: Chest logic

- code box overlay accepts letters only
- pressing Enter checks if input equals `KEY`
- correct input sets `codeBoxUnlocked = true`
- HUD update animates `codeBoxOpenProgress`

### Step 6: Key reveal

`room_codebox.cpp` reveals the key only after enough opening progress.

### Step 7: Escape

- picking up brass key adds it to inventory
- interacting with door checks inventory
- if key is present, door item becomes “opened”
- camera movement logic now allows travel outside

## 9. Good Viva Questions and Ready Answers

### Q1. Why did you split the code into multiple folders?

Answer:

To separate responsibilities clearly. `camera` handles movement, `app` handles rendering, `utils` handles shared systems, `objects` handles room props, and `room` handles room structure and puzzle-specific environment features.

### Q2. How are interactions implemented?

Answer:

Each interactable object is stored as an `Item` with name, position, radius, action, and text. The game finds the nearest relevant item in front of the player and runs custom logic based on that item.

### Q3. How is the puzzle managed?

Answer:

The puzzle is controlled through shared state variables like `drawerOpen`, `codeBoxUnlocked`, `codeBoxInput`, and inventory checks. The code uses direct gameplay rules instead of a formal class-based state machine because the project is intentionally compact.

### Q4. How are textures handled?

Answer:

The project uses both procedural textures generated in code and external PPM textures loaded from disk. Procedural textures are used for common materials, while unique assets like the skybox and painting use external image files.

### Q5. How are shadows implemented?

Answer:

They are simple projected floor shadows. The code projects object bounding-box corners from a light source onto the floor plane and draws a semi-transparent quad.

### Q6. Why use OpenGL fixed-function pipeline?

Answer:

Because it is easier for learning and demonstration. It lets the project focus on 3D scene construction, interaction, and gameplay flow without introducing shader complexity.

### Q7. What is the main software engineering strength of this project?

Answer:

Modularity. The logic is separated into rendering, input, state, interactions, room structure, and object-specific files. That makes it much easier to maintain and explain.

## 10. Short Project Summary for Interview

You can describe the project like this:

> This is a modular C++ OpenGL first-person escape room project. The player explores a 3D room, interacts with objects, collects clues, solves a chest code puzzle, obtains a key, and opens the exit door. The project uses GLUT for windowing and input, OpenGL fixed-function rendering for the scene, shared state for puzzle progression, procedural and external textures for materials, and a 2D HUD/overlay system for prompts and clues.

## 11. Final Takeaway

If you need to explain this project confidently in a viva, focus on these four points:

1. It is a **modular 3D interactive application** built in C++ and OpenGL.
2. The project combines **graphics, input handling, gameplay logic, and UI overlays**.
3. The puzzle system is simple but complete: **observe clues -> infer code -> unlock chest -> collect key -> open door**.
4. The codebase is organized so each file has a clear responsibility, which is exactly what makes the project maintainable and interview-friendly.