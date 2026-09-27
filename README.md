# AIRSTRIKER — 2D SPACE SHOOTER GAME

**College Integrated Project (Four-Subject Academic Engineering Project)**

A desktop 2D side-scrolling space shooter built to visibly demonstrate core academic concepts across Computer Science and Engineering disciplines.

---

## 1. Integrated Technology Stack & Academic Subjects

| Subject Area | Technology | Course Syllabus Mapping |
|---|---|---|
| **1. Programming Lab / Data Structures (PL)** | **C++ (Standard C++11)** | Contiguous Arrays, Memory Pooling, Inactive Slot Reclamation, Linear Search, Kinematics. |
| **2. Problem Solving using OOP (PSOOP)** | **Java (JDK 26)** | Classes, Objects, Constructors, Encapsulation, String Validation, Subclass Inheritance (`Player`, `Enemy`), Service Orchestration (`GameEngine`). |
| **3. Computer Organization & Architecture (COA)** | **80386 Simulator & ALP** | 32-bit Register File (`EAX`, `EDX`), Status Flags (`CF`, `ZF`, `SF`, `OF`, `PF`, `AF`), `ADD` and `SUB` Micro-operations, Branch Condition Codes (`JZ`, `JS`). |
| **4. Computer Graphics Lab (CGL)** | **C++ / OpenGL / GLUT** | Procedural Geometric Primitives (`GL_TRIANGLES`, `GL_QUADS`, `GL_POINTS`, `GL_LINES`), 2D Orthographic Projection, Alpha Blending, 60 FPS Animation. |

---

## 2. Project Directory Architecture

```
AirStriker/
├── ARCHITECTURE.md                     # Master System Architecture Document & Schematics
├── README.md                           # Master project manual and syllabus mapping
├── build_all.bat                       # One-click compiler for all four subject targets
├── run_cgl_game.bat                    # Launches the playable OpenGL desktop arcade game
├── run_pl_tests.bat                    # Executes C++ Bullet Pool Array automated test suite
├── run_psoop_tests.bat                 # Executes Java OOP domain model test suite
├── run_coa_tests.bat                   # Executes 80386 ALU arithmetic & flag test suite
│
├── PL/                                 # Programming Lab / Data Structures (C++)
│   ├── src/
│   │   ├── BulletPoolArray.h           # Fixed-size array bullet pool header
│   │   └── BulletPoolArray.cpp         # Slot recycling, boundary management & search
│   └── tests/
│       └── test_bullet_pool.cpp        # Unit tests verifying 10+ simultaneous bullets
│
├── PSOOP/                              # Problem Solving using OOP (Java)
│   ├── src/
│   │   ├── model/
│   │   │   ├── Character.java          # Base class with encapsulation & input validation
│   │   │   ├── Player.java             # Player ship subclass with shield absorption
│   │   │   ├── Enemy.java              # Alien hostile subclass with bounty tracking
│   │   │   ├── Bullet.java             # Projectile entity class
│   │   │   └── GameState.java          # Telemetry state snapshot model
│   │   ├── service/
│   │   │   └── GameEngine.java         # Central loop, wave spawning & collision sweep
│   │   └── test/
│   │       └── PSOOPTestRunner.java    # Standalone test runner (23 assertions)
│   └── data/
│
├── COA/                                # Computer Organization & Architecture (80386)
│   ├── ALU/
│   │   ├── ALU80386.h                  # 32-bit 80386 ALU Simulator header
│   │   ├── ALU80386.cpp                # ADD/SUB execution with accurate EFLAGS updates
│   │   └── test_alu.cpp                # ALU automated verification suite
│   ├── ALP/
│   │   ├── score_add.asm               # 80386 assembly source for score addition
│   │   └── health_sub.asm              # 80386 assembly source for damage & JZ/JS flags
│   ├── ControlUnit/                    # (Scheduled for CO-2)
│   └── README.md                       # Architectural mapping of CPU flags to game logic
│
├── CGL/                                # Computer Graphics Lab (C++ / OpenGL)
│   ├── src/
│   │   ├── Renderer.h                  # Procedural geometric renderer header
│   │   ├── Renderer.cpp                # Procedural primitives for ship, drone, stars, HUD
│   │   └── MainGame.cpp                # Playable game loop integrating PL, COA, & CGL
│   └── shaders/
│
├── data/                               # Shared data files
│   ├── game_state.json                 # Baseline state configuration
│   ├── players.csv                     # Pilot registry
│   └── leaderboard.csv                 # High scores table
│
└── docs/
    ├── architecture/                   # Architectural blueprints & component specifications
    │   ├── system_architecture.md      # In-depth multi-tier architecture & memory models
    │   └── component_specifications.md # Subsystem API contracts, invariants & algorithms
    ├── screenshots/                    # Gameplay screen captures
    └── progress/
        └── CO1.md                      # Comprehensive CO-1 progress milestone report
```

---

## 3. System Architecture & Visual Schematics

AirStriker features a dedicated architectural design layer that connects the four subjects:
- **[ARCHITECTURE.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/ARCHITECTURE.md):** Complete architectural specification containing visual Mermaid diagrams for:
  - 4-Tier Multi-Layer System Topology.
  - 60 FPS Deterministic Game Loop Sequence.
  - Bullet Pool Contiguous Memory Layout and $O(1)$ Recycling.
  - OOP Domain Model Class Hierarchy and Service Decoupling.
  - Hardware-Software Co-Design (ALU micro-operations & status flag condition codes).
  - Procedural OpenGL Graphics Pipeline.
  - System Lifecycle State Machine.
- **[docs/architecture/system_architecture.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/architecture/system_architecture.md):** In-depth engineering blueprint covering projection matrices, cache-friendly memory models, and deterministic single-threaded concurrency.
- **[docs/architecture/component_specifications.md](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/architecture/component_specifications.md):** Formal API specifications, complexity analyses, and operational invariants for each subsystem.

---

## 4. Current Progress Checklist

### CO-1 Milestone (Foundation + Basic Game)
- [x] **Project Setup**: Standardized 4-subject modular directory structure initialized.
- [x] **Player**: Fully encapsulated player ship with movement, health, and shield.
- [x] **Enemy**: Hostile alien drone squad with kinematic patrol and collision damage.
- [x] **Bullet**: Projectile system with velocity vectors and boundary destruction.
- [x] **Movement**: Responsive 2D keyboard controls with screen clamping.
- [x] **Shooting**: Rate-limited blaster firing laser bolts from ship nose.
- [x] **Bullet Array**: Fixed 64-slot pool in C++ (`BulletPoolArray`) with $O(1)$ slot reuse.
- [x] **Collision**: AABB collision sweeps for projectile-enemy and player-enemy hits.
- [x] **Score**: Dynamic scoring via 80386 ALU addition micro-operations.
- [x] **Health**: Shield absorption and hull damage via 80386 ALU subtraction with flag tracking.
- [x] **OpenGL Rendering**: Double-buffered window with procedural geometric primitives and HUD.
- [x] **ALU Demonstration**: C++ 80386 simulator and 80386 ALP programs with flag monitoring.
- [x] **Testing**: Automated unit tests for PL, PSOOP, and COA all passing at 100%.
- [x] **Documentation**: Complete milestone report in `docs/progress/CO1.md`.

---

## 5. How to Build & Run

### Prerequisites
- **MinGW GCC / G++** with OpenGL and FreeGLUT (`g++`, `libglut32.a`, `glut32.dll`).
- **Java Development Kit (JDK)** (`javac`, `java`).

### One-Click Build
Run the master build script from the project root:
```bat
build_all.bat
```
This compiles:
1. `bin/test_bullet_pool.exe` (PL test suite)
2. `bin/psoop/test/PSOOPTestRunner.class` (PSOOP test suite)
3. `bin/test_alu.exe` (COA test suite)
4. `bin/AirStriker_CO1.exe` (CGL integrated playable game)

### Running the Playable Game
To launch the 800x600 OpenGL arcade desktop game:
```bat
run_cgl_game.bat
```
*(Or directly launch `bin\AirStriker_CO1.exe`)*

### Running Subject Test Suites
- **Programming Lab (PL)**:
  ```bat
  run_pl_tests.bat
  ```
- **Problem Solving using OOP (PSOOP)**:
  ```bat
  run_psoop_tests.bat
  ```
- **Computer Organization & Architecture (COA)**:
  ```bat
  run_coa_tests.bat
  ```

---

## 6. Controls

| Key | In-Game Action |
|---|---|
| **W / Up Arrow** | Move ship Up |
| **S / Down Arrow** | Move ship Down |
| **A / Left Arrow** | Move ship Left |
| **D / Right Arrow** | Move ship Right |
| **SPACE** | Fire High-Energy Laser Cannon |
| **R** | Reboot Flight Computer / Restart after Game Over |
| **ESC** | Exit Game |

---

## 7. Syllabus Course Outcome (CO) Mapping

- **CO-1 [COMPLETED]**: Foundation + Basic Game (Array Bullet Pool, OOP Fundamentals, 80386 ALU Arithmetic, OpenGL Primitives).
- **CO-2 [NEXT]**: Field + OOP + Basic Graphics (2D Arrays, Asteroid Field, Inheritance `Character -> Player / Enemy -> Drone / Fighter / Boss`, Control Unit, Bresenham Line & Circle).
- **CO-3**: Combat System (Singly/Doubly Linked Lists, Combo Chain, Abstract Classes & Interfaces, 64-bit Arithmetic, Scan-line Polygon Fill).
- **CO-4**: Waves + Power-Ups (Circular Linked List, Stack, Circular Queue, Deque, Custom Exceptions, Multiplication ALP, Transformations).
- **CO-5**: Advanced Systems (Searching, Sorting, Hashing, File I/O, Generics, Hex <-> BCD, Cohen-Sutherland Clipping, Textures).
- **CO-6**: Complete Integration (Full four-subject integrated game flow, final leaderboard, and polish).
