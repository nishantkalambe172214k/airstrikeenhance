# PROJECT BRAIN & MASTER CONTEXT PRIMER
# AIRSTRIKER — 2D SPACE SHOOTER GAME (COLLEGE INTEGRATED PROJECT)

> **Purpose of this file (`brain.md`):**  
> This file contains the complete architectural, technical, syllabus, and progress context of the **AirStriker** project. Whenever this document is shared with ChatGPT, Claude, Gemini, or an academic evaluator, the AI/reader will instantly understand the entire history, codebase structure, technology rules, syllabus mappings, and exactly how to continue development without missing any context.

---

## 1. PROJECT EXECUTIVE SUMMARY & MISSION

- **Project Title:** AIRSTRIKER — 2D Space Shooter Game
- **Project Type:** College Integrated Engineering Capstone Project
- **Target Platform:** Desktop Windows (x86/x64)
- **Primary Goal:** Build a desktop 2D side-scrolling arcade space shooter where the player pilots an interceptor, maneuvers through asteroid fields, destroys hostile alien drone waves, manages shields and hull integrity, collects power-ups, defeats bosses, and earns high scores on a leaderboard.
- **Core Engineering Mandate:** Every required academic concept must be implemented meaningfully and mapped to an active gameplay mechanic. **Fake, demo-only, or dummy placeholder implementations are strictly forbidden.**
- **Technology Rule:**
  - **PL (Programming Lab):** Pure C++ (Data Structures).
  - **PSOOP (Object-Oriented Programming):** Pure Java (JDK 26).
  - **COA (Computer Organization & Architecture):** YASMIN CPU-OS Simulator (Version 7.5.50 by Besim Mustafa) / 80386 ALP with status flag condition codes.
  - **CGL (Computer Graphics Lab):** C++ with native OpenGL / GLUT (Procedural geometric primitives; no prebuilt game engines like Unity, Unreal, Godot, or Pygame).

---

## 2. FOUR-SUBJECT INTEGRATION MATRIX

| Academic Subject | Technology | Primary Syllabus Concepts | Mapped Game Feature |
|---|---|---|---|
| **1. Programming Lab / Data Structures (PL)** | **C++ (C++11)** | Static Arrays, 2D Arrays, Linked Lists (Singly, Doubly, Circular), Stack, Queues (Circular Queue, Deque), Recursion, Searching, Sorting, Hashing. | Bullet Pool, Asteroid/Obstacle Grid, Combo Chains, Wave Queues, Weapon Selection Stack, High-Score Hashing & Sorting. |
| **2. Problem Solving using OOP (PSOOP)** | **Java (JDK 26)** | Classes, Objects, Constructors, Encapsulation, Inheritance, Abstract Classes, Interfaces, Polymorphism, Exception Handling, File I/O, Generics, Collections (`Repository<T>`). | Domain Model (`Character`, `Player`, `Enemy` -> `Drone`/`Fighter`/`Boss`), Interfaces (`Movable`, `Collidable`, `Shootable`), Game State snapshots, CSV Leaderboard persistence. |
| **3. Computer Organization & Architecture (COA)** | **YASMIN CPU-OS Simulator & 80386 ALP** | 32-bit Registers (`R00`-`R31` in YASMIN / `EAX`, `EDX` in x86), Status Flags (`Z`/`ZF`, `N`/`SF`, `OV`/`OF`, `CF`), Arithmetic (`ADD`, `SUB`, `MUL`, `DIV`), Control Unit state machine, BCD conversion. | Score accumulation (`ADD`), Hull damage (`SUB`), Fatal breach / Game Over flag trigger (`ZF=1`), Damage/Combo multipliers, BCD score display formatting. |
| **4. Computer Graphics Lab (CGL)** | **C++ / OpenGL / GLUT** | Double-buffered window, 2D Orthographic Projection, Geometric Primitives (`GL_TRIANGLES`, `GL_QUADS`, `GL_POINTS`, etc.), Bresenham Line & Circle, Transformations, Polygon Fill, Clipping, Texturing. | Spaceship geometry, Pulsating thruster flare, Alien drone models, Parallax starfield, Laser cannons, Health rings, Radar minimap, Screen clipping. |

---

## 3. SIX-COURSE OUTCOME (CO) DEVELOPMENT ROADMAP

The project is executed strictly in 6 sequential Course Outcome (CO) milestones:

### CO-1 — FOUNDATION + BASIC GAME [COMPLETED]
- **PL:** Contiguous static arrays; Fixed-size Bullet Pool (`BulletPoolArray`); $O(1)$ slot allocation & recycling; Linear search; Boundary reclamation.
- **PSOOP:** Java fundamentals, encapsulation, string validation, constructors, `Character`, `Player`, `Enemy`, `Bullet`, `GameEngine`, `GameState`.
- **COA:** ALU Design; Addition & Subtraction micro-operations; Status Flags (`ZF`, `SF`, `CF`, `OF`); YASMIN CPU-OS Simulator practical implementation + x86 ALP + C++ real-time software ALU emulation.
- **CGL:** OpenGL setup, 2D orthographic projection, procedural geometric primitives (player ship, enemies, laser bolts, parallax starfield, arcade HUD).
- **Result:** Fully playable 60 FPS desktop arcade game prototype (`AirStriker_CO1.exe`) with WASD movement, spacebar shooting, collision detection, health gauge, score tracking, and game over screen.

### CO-2 — FIELD + OOP + BASIC GRAPHICS [NEXT]
- **PL:** 2D Arrays; Procedural Asteroid / Obstacle Field matrix.
- **PSOOP:** Inheritance hierarchy refinement; `Character -> Player / Enemy`; `Enemy -> Drone / Fighter / Boss`; Keywords: `static`, `final`, `super`, `this`.
- **COA:** Control Unit architecture; State machine modeling Instruction Fetch -> Decode -> Execute mapped to `Input -> Update -> Render`.
- **CGL:** Mid-point Bresenham Line and Bresenham Circle drawing algorithms for laser targeting reticles, circular health rings, and radar/minimap rings.

### CO-3 — COMBAT SYSTEM
- **PL:** Singly Linked List (active hostile entities), Doubly Linked List (combo chains and score merging).
- **PSOOP:** Abstract classes, Interfaces (`Movable`, `Collidable`, `Shootable`), dynamic polymorphism (distinct `move()` behaviors).
- **COA:** 64-bit wide arithmetic micro-operations for high-score overflows.
- **CGL:** Scan-line polygon filling algorithms and procedural explosion spark particles.

### CO-4 — WAVES + POWER-UPS
- **PL:** Circular Linked List (infinite enemy waves), Circular Queue (incoming enemy squads), Stack (weapon upgrade levels), Deque (recent player actions), Recursion (obstacle field partitioning).
- **PSOOP:** Custom exception handling (`WeaponOverheatException`, `ShieldDepletedException`), Java Collections Framework.
- **COA:** Multiplication ALP for combo and score multipliers.
- **CGL:** 2D Affine Transformations (Translation, Rotation, Scaling), multi-layer parallax scrolling, animated power-up orbs.

### CO-5 — ADVANCED SYSTEMS
- **PL:** Searching algorithms (Binary search on scores), Sorting algorithms (QuickSort / MergeSort on leaderboard), Hashing (Hash table for player IDs).
- **PSOOP:** File handling (CSV/JSON read-write), Generics (`Repository<T>`), Collections.
- **COA:** Hexadecimal to BCD (Binary Coded Decimal) conversion ALP for electronic score displays.
- **CGL:** Cohen-Sutherland 2D line clipping for off-screen objects, texture mapping on geometric meshes.

### CO-6 — COMPLETE INTEGRATION
- Unified integration of all four subjects into the final release build.
- Complete game mission flow: Start Mission -> Wave Combat -> Power-Ups -> Boss Battle -> Mission Complete -> High-Score Sorting -> Save Data.

---

## 4. COMPLETE CODEBASE DIRECTORY STRUCTURE

```
AirStriker/
├── brain.md                            # [THIS FILE] Master Project Brain & Context Primer
├── ARCHITECTURE.md                     # Master System Architecture Document & Schematics
├── README.md                           # User manual, syllabus mapping, and build instructions
├── build_all.bat                       # CMD master compilation script (builds all 4 targets)
├── build_all.ps1                       # PowerShell master compilation script
├── run_cgl_game.bat                    # CMD launcher for playable desktop game
├── run_game.ps1                        # PowerShell native launcher for playable desktop game
├── run_pl_tests.bat                    # Automated test runner for PL C++ Bullet Pool
├── run_psoop_tests.bat                 # Automated test runner for PSOOP Java classes
├── run_coa_tests.bat                   # Automated test runner for COA 80386 ALU Simulator
├── glut32.dll                          # OpenGL Utility Toolkit runtime library for Windows
│
├── bin/                                # Compiled Executable Binaries & Bytecode
│   ├── AirStriker_CO1.exe              # Playable 60 FPS Desktop Game (CGL + PL + COA)
│   ├── test_bullet_pool.exe            # PL C++ test executable
│   ├── test_alu.exe                    # COA ALU test executable
│   ├── glut32.dll                      # Local DLL copy ensuring portable execution
│   └── psoop/                          # Compiled Java Bytecode Directory
│       ├── model/                      # Character.class, Player.class, Enemy.class, Bullet.class, GameState.class
│       ├── service/                    # GameEngine.class
│       └── test/                       # PSOOPTestRunner.class
│
├── PL/                                 # Programming Lab (C++)
│   ├── src/
│   │   ├── BulletPoolArray.h           # Header: Bullet struct and BulletPoolArray class
│   │   └── BulletPoolArray.cpp         # Implementation: Fixed-array pool, slot reuse, linear search
│   └── tests/
│       └── test_bullet_pool.cpp        # Test suite: 15 simultaneous bullets, kinematics, recycling
│
├── PSOOP/                              # Problem Solving using OOP (Java)
│   ├── src/
│   │   ├── model/
│   │   │   ├── Character.java          # Base class with private encapsulation & input validation
│   │   │   ├── Player.java             # Subclass: Shield absorption, score, boundary clamping
│   │   │   ├── Enemy.java              # Subclass: Drone archetype, bounty points, collision damage
│   │   │   ├── Bullet.java             # Projectile entity model
│   │   │   └── GameState.java          # Snapshot container for all telemetry variables
│   │   ├── service/
│   │   │   └── GameEngine.java         # Central engine: wave spawn, input handling, AABB collision sweep
│   │   └── test/
│   │       └── PSOOPTestRunner.java    # Standalone Java test suite (23 assertions passed)
│   └── data/
│
├── COA/                                # Computer Organization & Architecture
│   ├── README.md                       # Comprehensive COA verification, syllabus mapping & architecture
│   ├── ALU/
│   │   ├── ALU80386.h                  # 32-bit register file and EFLAGS simulation header
│   │   ├── ALU80386.cpp                # Software ALU ADD/SUB with flag calculation (ZF, SF, CF, OF)
│   │   └── test_alu.cpp                # ALU test suite verifying arithmetic and flag triggers
│   ├── ALP/
│   │   ├── yasmin_score_add.asm        # YASMIN CPU-OS Simulator assembly for score addition (R00, R01)
│   │   ├── yasmin_health_sub.asm       # YASMIN CPU-OS Simulator assembly for non-lethal damage
│   │   ├── yasmin_health_lethal.asm    # YASMIN CPU-OS Simulator assembly triggering Zero Flag (Z=1)
│   │   ├── score_add.asm               # Intel 80386 MASM assembly reference (ADD EAX, EDX)
│   │   └── health_sub.asm              # Intel 80386 MASM assembly reference (SUB EAX, EDX, JZ/JS)
│   ├── simulator/
│   │   └── CPU - OS Simulator 7-5-50.exe # Official installer downloaded from teach-sim.com
│   └── vm/
│       ├── cpu_vm.exe                  # Compiled standalone official Virtual Machine from teach-sim
│       ├── Instruction Library.h / .cpp # Official VM instruction execution engine
│       ├── Support Library.h / .cpp    # Memory and bytecode loader
│       └── Virtual Machine.h / .cpp    # VM entry point and runtime interpreter
│
├── CGL/                                # Computer Graphics Lab (C++ / OpenGL)
│   ├── src/
│   │   ├── Renderer.h                  # Procedural geometric renderer header
│   │   ├── Renderer.cpp                # Pure primitive rendering (fuselage, wings, thrusters, HUD)
│   │   └── MainGame.cpp                # 60 FPS Desktop Game Loop integrating PL and COA
│   └── shaders/
│
├── data/                               # Shared Game Telemetry & Persistence Files
│   ├── game_state.json                 # Shared state schema baseline
│   ├── players.csv                     # Registered pilot callsigns and statistics
│   └── leaderboard.csv                 # High scores and wave records
│
└── docs/                               # Documentation & Archival Media
    ├── architecture/                   # Architectural blueprints & component specifications
    │   ├── system_architecture.md      # In-depth multi-tier architecture & memory models
    │   └── component_specifications.md # Subsystem API contracts, invariants & algorithms
    ├── screenshots/
    │   └── coa/                        # Verified screenshots from YASMIN CPU-OS Simulator
    │       ├── yasmin_instruction_entry.png
    │       ├── yasmin_hlt_instruction.png
    │       ├── yasmin_alu_addition_result.png    (ADD R00, R01 -> R01=60)
    │       ├── yasmin_alu_subtraction_result.png (SUB R00, R01 -> R01=20)
    │       ├── yasmin_alu_multiplication_result.png (MUL R00, R01 -> R01=800)
    │       └── yasmin_alu_division_result.png
    └── progress/
        └── CO1.md                      # Detailed CO-1 milestone report
```

---

## 5. MASTER ARCHITECTURAL BLUEPRINTS & SCHEMATICS

The project architecture is formally specified in dedicated documents:
- **[ARCHITECTURE.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/ARCHITECTURE.md):** Master architecture document with visual Mermaid diagrams for 4-tier system topology, 60 FPS game loop lifecycle, PL bullet pool memory model, PSOOP class hierarchy, COA hardware-software co-design, and state machine.
- **[docs/architecture/system_architecture.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/architecture/system_architecture.md):** In-depth engineering blueprint covering projection matrices, cache locality, and single-threaded deterministic synchronization.
- **[docs/architecture/component_specifications.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/architecture/component_specifications.md):** Subsystem API contracts, invariants, time/space complexities, and algorithm specs.

---

## 6. TECHNICAL DEEP DIVES PER SUBJECT

### A. Programming Lab (PL) — C++
- **File:** [`PL/src/BulletPoolArray.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/src/BulletPoolArray.cpp)
- **Problem Solved:** Dynamic memory allocation (`new`/`delete`) in a 60 FPS game loop causes memory fragmentation, garbage pauses, and unpredictable frame pacing.
- **Solution — Fixed-Size Array Pool:**
  - Contiguous buffer: `Bullet bullets[MAX_BULLETS];` (capacity = 64).
  - Slot Allocation (`fireBullet`): Linearly traverses the array for `active == false`. When found, claims the slot in $O(1)$ relative to available slots, initializes parameters, and returns the assigned ID.
  - Kinematic Update (`updateBullets`): Iterates sequentially through memory. Updates `x += vx`, `y += vy`. Automatically reclaims slots (`active = false`) when coordinates exceed screen bounds `[0, 800] x [0, 600]`.
  - Linear Search (`searchBullet`): Demonstrates classic array searching by `bulletID`.
  - Slot Recycling: Reclaims deactivated slots immediately without memory reallocation.

### B. Problem Solving using OOP (PSOOP) — Java
- **Files:** `PSOOP/src/model/*.java`, `PSOOP/src/service/GameEngine.java`
- **Class Hierarchy:**
  ```
  Character (Base Class)
  ├── Player (Subclass: shield absorption, score tally, lives management)
  └── Enemy (Subclass: drone archetype, bounty value, collision damage)
  ```
- **Key OOP Principles:**
  - **Encapsulation:** Private fields accessed only via validated getters and setters.
  - **Defensive Input Validation:** Callsign strings must be non-null, non-whitespace, and between 2 and 25 characters; health and speed must be strictly non-negative.
  - **Shield Absorption Logic:** Player takes damage by first deducting from `shield`. Only residual unabsorbed damage is passed to `health`.
  - **GameEngine Service:** Decouples game logic from presentation. Orchestrates player state, enemy squads, projectile lists, and AABB collision resolution.

### C. Computer Organization & Architecture (COA)
- **Crucial Syllabus Distinction:**
  - **The College Lab Simulator:** The student's actual college practical (*COA Practical 1: Design your ALU using CPU simulator for arithmetic operations*) uses **YASMIN CPU-OS Simulator (Version 7.5.50 by Besim Mustafa, Edge Hill University)**.
  - **YASMIN Architecture:** Uses 32 registers `R00`-`R31`, status flags `Z` (Zero), `N` (Negative), `OV` (Overflow), and instructions `MOV #val, Rxx`, `ADD Rxx, Ryy`, `SUB Rxx, Ryy`, `HLT`.
  - **Incompatibility Note:** Standard Intel x86 MASM `.asm` files cannot run directly inside YASMIN because YASMIN uses its own educational RISC instruction set.
  - **Complete Solution Provided:**
    1. **YASMIN Assembly Programs:** `yasmin_score_add.asm`, `yasmin_health_sub.asm`, `yasmin_health_lethal.asm` (for evaluation on the college simulator).
    2. **Verified Simulator Screenshots:** Archived in `docs/screenshots/coa/` proving genuine execution of Addition (`20+40=60`), Subtraction (`40-20=20`), and Multiplication (`20*40=800`).
    3. **Official VM Compiled:** Compiled `COA/vm/cpu_vm.exe` from official C++ source code.
    4. **x86 Reference ALPs:** `score_add.asm` and `health_sub.asm` for 80386 protected mode assembly reference.
    5. **Game Engine C++ ALU:** `ALU80386.cpp` used inside `MainGame.cpp` to calculate score increments and damage in real-time frame cycles.

### D. Computer Graphics Lab (CGL) — C++ / OpenGL
- **Files:** [`CGL/src/Renderer.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/Renderer.cpp), [`CGL/src/MainGame.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/MainGame.cpp)
- **Pure Procedural Geometry (No External Bitmaps):**
  - **Player Interceptor:** Sleek delta-wing starfighter rendered with `GL_TRIANGLES` (electric blue fuselage), `GL_QUADS` (swept wings), `GL_POLYGON` (cockpit diamond canopy), and animated flickering orange/yellow plasma exhaust.
  - **Alien Drone Hostiles:** Crimson forward-swept wedge geometry with pulsating central red sensor core.
  - **Laser Blasters:** Luminous cyan dual-layer energy bolts (`GL_QUADS` outer glow + inner bright white core).
  - **Parallax Starfield:** 90 procedural stars drifting across 3 velocity tiers using `GL_POINTS`.
  - **Arcade HUD:** Dynamic health bar (color-interpolated green -> yellow -> red based on percentage), score counter, current wave, and live bullet pool monitor rendered via `glutBitmapCharacter`.

---

## 7. HOW TO BUILD AND RUN

### Environment Requirements
- **Compiler:** MinGW GCC/G++ (`g++`, `gcc`).
- **Graphics:** OpenGL and FreeGLUT (`libglut32.a`, `glut32.dll` located in `bin\`).
- **Java:** JDK 17 or higher (`javac`, `java`).

### Master One-Click Build
```bat
# From Command Prompt (CMD):
build_all.bat

# From PowerShell:
.\build_all.ps1   (or cmd /c build_all.bat)
```

### Launching the Game
```bat
# From Command Prompt (CMD):
run_cgl_game.bat

# From PowerShell (requires .\ prefix):
.\run_game.ps1
# or
.\bin\AirStriker_CO1.exe
```

### Running Automated Test Suites
```bat
# 1. Programming Lab (PL) Bullet Pool tests:
.\run_pl_tests.bat

# 2. Problem Solving using OOP (PSOOP) Java tests:
.\run_psoop_tests.bat

# 3. Computer Organization & Architecture (COA) ALU tests:
.\run_coa_tests.bat
```

### Player Controls
- **W / Up Arrow:** Move ship Up
- **S / Down Arrow:** Move ship Down
- **A / Left Arrow:** Move ship Left
- **D / Right Arrow:** Move ship Right
- **SPACE:** Fire Laser Cannon (claims slot from `BulletPoolArray`)
- **R:** Reboot flight computer / Restart after Game Over
- **ESC:** Exit game

---

## 8. INSTRUCTIONS FOR ANY SUBSEQUENT AI (ChatGPT / Claude / Gemini)

When a user shares this project or asks for the next steps, follow these strict directives:

1. **Do NOT Restart or Overwrite CO-1:**
   - CO-1 is 100% complete, fully tested, and operational.
   - Respect the existing project architecture, directory structure, and separation of concerns.
2. **Next Target is STRICTLY CO-2:**
   - Do NOT skip to CO-3, CO-4, CO-5, or CO-6.
   - Implement only CO-2 features in order:
     - **PL:** 2D Arrays (Procedurally generated Asteroid/Obstacle Grid).
     - **PSOOP:** Refine inheritance hierarchy (`Character -> Player / Enemy -> Drone / Fighter / Boss`) utilizing `static`, `final`, `super`, `this`.
     - **COA:** Control Unit architecture mapping Instruction Cycle (`Fetch -> Decode -> Execute`) to Game Cycle (`Input -> Update -> Render`).
     - **CGL:** Mid-point Bresenham Line and Bresenham Circle drawing algorithms for targeting sights, health rings, and radar/minimap rings.
3. **Maintain Technology Rules:**
   - PL = C++
   - PSOOP = Java
   - COA = YASMIN CPU-OS Simulator / 80386 ALP
   - CGL = C++ with native OpenGL / GLUT
   - No third-party game engines (no Unity, Godot, Pygame).
4. **Zero Tolerance for Placeholders:**
   - Never write `// TODO implement`, dummy mockups, or fake calculations.
   - Every file must compile and run cleanly with actual code.
