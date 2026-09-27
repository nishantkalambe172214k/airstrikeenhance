# Progress Checkpoint: CO-1 (Foundation + Basic Game)

**Course Integrated Project:** AIRSTRIKER — 2D Space Shooter Game  
**Target Milestone:** CO-1 (Course Outcome 1)  
**Status:** Completed & Fully Verified  

---

## 1. CO-1 Objective
The primary objective of CO-1 is to establish the fundamental architecture of the desktop 2D space shooter, setting up the complete four-subject technological stack:
- High-performance memory pooling in C++ for projectile lifecycles (PL).
- Clean object-oriented domain models in Java with strict encapsulation and input validation (PSOOP).
- Authentic cycle-accurate simulation and assembly programs for 80386 ALU arithmetic and flag status conditions (COA).
- Procedural OpenGL primitive-based rendering pipeline for an interactive, 60 FPS arcade desktop game window (CGL).

---

## 2. Concepts Implemented

| Subject | Academic Discipline | Syllabus Concepts Implemented in CO-1 |
|---|---|---|
| **PL** | Programming Lab / Data Structures (C++) | Contiguous static arrays (`bullets[MAX_BULLETS]`), fixed-capacity bullet pooling, O(1) slot allocation and recycling, linear search (`searchBullet`), kinematic updates, boundary-based slot reclamation. |
| **PSOOP** | Problem Solving using OOP (Java) | Java fundamentals, classes, objects, parameterized/default constructors, private access modifiers, encapsulation, validated getters/setters, string handling, input validation, subclass inheritance (`Player`, `Enemy` extending `Character`), service orchestration (`GameEngine`). |
| **COA** | Computer Organization & Architecture | 80386 32-bit register file (`EAX`, `EDX`), Status Flag Register modeling (`CF`, `ZF`, `SF`, `OF`, `PF`, `AF`), ALU addition micro-operation (`ADD`), ALU subtraction micro-operation (`SUB`), condition codes for defeat branches (`JZ`, `JS`, `JC`). |
| **CGL** | Computer Graphics Lab (C++ / OpenGL) | Double-buffered OpenGL window setup, 2D orthographic projection (`gluOrtho2D`), geometric primitive rendering (`GL_POINTS`, `GL_LINES`, `GL_TRIANGLES`, `GL_QUADS`, `GL_POLYGON`, `GL_LINE_LOOP`), color gradients, alpha blending, parallax scrolling, 60 FPS animation timer (`glutTimerFunc`). |

---

## 3. Game Features Implemented
1. **Interactive Game Window**: 800x600 desktop arcade window with smooth 60 FPS double-buffered rendering.
2. **Player Spaceship**: Geometric interceptor with multi-polygon aerodynamic fuselage, swept delta wings, glass cockpit canopy, and animated plasma thruster plume.
3. **Responsive Controls**: Smooth multi-key input handling with directional translation (WASD and Arrow keys) and screen boundary clamping.
4. **Active Laser Blasters**: Fast cyan laser bolts fired from the player ship with rate-limiting cadence.
5. **Fixed Array Bullet Pool**: Bullets managed in a pre-allocated fixed array of 64 slots with automatic slot recycling.
6. **Hostile Alien Drones**: Crimson angular wedge scout drones moving in formation towards the player's base.
7. **Collision Detection & Resolution**: Axis-Aligned Bounding Box (AABB) hit tests for bullet-enemy strikes and player-enemy ramming impacts.
8. **Real-time Health & Shield**: Shield absorbs preliminary damage before residual damage penetrates to player hull integrity.
9. **Dynamic Arcade HUD**: Real-time health gauge (color interpolated green-to-red), score tally, current wave counter, and live bullet pool slot telemetry.
10. **Game Over & Reboot**: Distinct mission failure overlay screen triggered upon hull breach; press `[R]` to reboot systems and retry immediately.

---

## 4. Subject Contributions

### Programming Lab (PL) Contribution
- **Source Files:** [`PL/src/BulletPoolArray.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/src/BulletPoolArray.h), [`PL/src/BulletPoolArray.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/src/BulletPoolArray.cpp)
- **Test File:** [`PL/tests/test_bullet_pool.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/tests/test_bullet_pool.cpp)
- **Technical Role:**
  Eliminates heap allocation (`new`/`delete`) inside the 60 FPS render loop. A fixed array `Bullet bullets[MAX_BULLETS]` provides $O(1)$ inactive slot reclamation when firing, sequential memory access during coordinate updates, and boundary-triggered reclamation.
- **Verification:** Automated tests verify firing 15 simultaneous projectiles, kinematic position updates, linear search by ID, slot reuse, and off-screen boundary deactivation.

### Problem Solving using OOP (PSOOP) Contribution
- **Source Files:** 
  - [`PSOOP/src/model/Character.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/model/Character.java)
  - [`PSOOP/src/model/Player.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/model/Player.java)
  - [`PSOOP/src/model/Enemy.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/model/Enemy.java)
  - [`PSOOP/src/model/Bullet.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/model/Bullet.java)
  - [`PSOOP/src/model/GameState.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/model/GameState.java)
  - [`PSOOP/src/service/GameEngine.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/service/GameEngine.java)
- **Test File:** [`PSOOP/src/test/PSOOPTestRunner.java`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PSOOP/src/test/PSOOPTestRunner.java)
- **Technical Role:**
  Establishes the clean object hierarchy. `Character` enforces encapsulation with bounds checking on health, speed, and callsign string validation (non-null, length between 2 and 25). `Player` adds shield mechanics and score tracking. `GameEngine` manages simulation steps, collision sweeps, and state synchronization.
- **Verification:** 23/23 assertions passed covering validation exceptions, damage absorbing order, and simulation steps.

### Computer Organization & Architecture (COA) Contribution
- **Designated Syllabus Simulator:** **YASMIN CPU-OS Simulator** (Version: 7.5.50, by Besim Mustafa, Edge Hill University, UK)
- **Syllabus Practical Mapping:** COA Lab Practical No. 1 — *Design your ALU using CPU simulator for arithmetic operations*
- **Syllabus Simulator Programs:**
  - [`COA/ALP/yasmin_score_add.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_score_add.asm) — Score accumulation using `MOV #250, R00`, `MOV #1500, R01`, `ADD R00, R01`, `HLT`.
  - [`COA/ALP/yasmin_health_sub.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_health_sub.asm) — Non-lethal damage reduction using `SUB R00, R01`.
  - [`COA/ALP/yasmin_health_lethal.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_health_lethal.asm) — Lethal damage setting Status Register Zero Flag (`Z=1`).
- **Intel 80386 ALP References:**
  - [`COA/ALP/score_add.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/score_add.asm)
  - [`COA/ALP/health_sub.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/health_sub.asm)
- **C++ Game Emulation Layer (Separated from Practical):**
  - [`COA/ALU/ALU80386.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALU/ALU80386.h) & [`COA/ALU/ALU80386.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALU/ALU80386.cpp)
  - Simulates the arithmetic in real-time within the OpenGL game process (`MainGame.cpp`).
- **Official VM Engine:**
  - Compiled [`COA/vm/cpu_vm.exe`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/vm/cpu_vm.exe) from official C++ source.
- **Simulator Screenshots Archive:**
  - Saved in [`docs/screenshots/coa/`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/screenshots/coa/) demonstrating verified addition (`R01=60`), subtraction (`R01=20`), and multiplication (`R01=800`).

### Computer Graphics Lab (CGL) Contribution
- **Source Files:**
  - [`CGL/src/Renderer.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/Renderer.h)
  - [`CGL/src/Renderer.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/Renderer.cpp)
  - [`CGL/src/MainGame.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/MainGame.cpp)
- **Technical Role:**
  Provides the interactive desktop game. Configures the orthographic projection, implements procedural geometric rendering without external sprites, scrolls multi-depth starfields with variable point sizes, and executes the integrated game loop.

---

## 5. Architectural Blueprint
```
+-----------------------------------------------------------------------+
|                 AIRSTRIKER 2D DESKTOP GAME WINDOW                     |
|                                                                       |
|  [CGL Renderer] Procedural OpenGL Primitives (Ship, Stars, Laser)     |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    MAIN GAME LOOP (MainGame.cpp)                      |
|                                                                       |
|  1. Input Capture (WASD, Arrows, Space, R, ESC)                       |
|  2. Kinematic Positions Update                                        |
|  3. AABB Collision Sweep                                              |
|                                                                       |
|  [PL Bullet Pool] --------------------> [COA 80386 ALU]               |
|  Fixed-Size Array Allocation            ADD EAX, EDX (Score Accum)    |
|  O(1) Slot Recycling                    SUB EAX, EDX (Health / Flags) |
+-----------------------------------------------------------------------+
                                   |
                  (Mirrored Architecture & Model)
                                   v
+-----------------------------------------------------------------------+
|              PSOOP JAVA DOMAIN MODEL & GAME ENGINE                    |
|                                                                       |
|  Character -> Player (Shield, Score, Callsign Validation)             |
|  Character -> Enemy (Drone Archetype, Bounty Points)                  |
|  Bullet (Kinematics, Boundary Check)                                  |
|  GameEngine (Service Orchestration & Telemetry)                       |
+-----------------------------------------------------------------------+
```

---

## 6. Testing Results Summary

| Subject Test Suite | Executable / Runner | Tests Executed | Status |
|---|---|---|---|
| **PL** | `bin/test_bullet_pool.exe` | 4 Major suites (15 simultaneous bullets, Kinematics, Slot Reuse, Boundary Reclaim) | **PASSED (100%)** |
| **PSOOP** | `bin/psoop/test/PSOOPTestRunner.class` | 23 assertions (Validation, Encapsulation, Shield Absorption, Simulation) | **PASSED (100%)** |
| **COA** | `bin/test_alu.exe` | 80386 Score Addition, Carry Rollover, Health Subtraction, Zero & Sign Flags | **PASSED (100%)** |
| **CGL** | `bin/AirStriker_CO1.exe` | Interactive window launch, 60 FPS animation, player motion, laser firing, HUD | **VERIFIED RUNNING** |

---

## 7. Visual Elements & UI Layout
- **Resolution**: 800 x 600 pixels, double-buffered RGB.
- **Top HUD**:
  - `HULL INTEGRITY: [HP]%` with dynamically shaded health bar (green -> yellow -> red).
  - `SCORE: [SCORE]` in vibrant arcade yellow.
  - `WAVE: [WAVE]` in electric cyan.
  - `BULLET POOL: [ACTIVE]/64` in steel blue.
- **Battlefield**:
  - Player Ship on left flank (cyan fuselage, metallic navy delta wings, diamond cockpit canopy, pulsating orange/yellow plasma plume).
  - Alien Drones on right flank (crimson forward wedge, red sensor eye).
  - Lasers: High-intensity cyan beams with core plasma.
  - Background: 90 dynamic stars across 3 velocity tiers.
- **Bottom HUD**: Controls guide banner.
- **Game Over**: Translucent red-bordered modal with final score tally and retry instructions.

---

## 8. Remaining Work & Roadmap for CO-2
In accordance with the 6 CO development syllabus:
- **PL**: Transition from 1D array to 2D Arrays; implement procedurally generated Obstacle / Asteroid Field matrix.
- **PSOOP**: Expand inheritance hierarchy with static/final keywords (`super`, `this`); implement differentiated enemy specializations (`Drone`, `Fighter`, `Boss`).
- **COA**: Implement Control Unit state machine modeling instruction fetch, decode, and execute cycles mapped to Input -> Update -> Render.
- **CGL**: Implement mid-point Bresenham Line and Bresenham Circle drawing algorithms for laser targeting sights, health rings, and radar/minimap rings.
