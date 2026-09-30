# Progress Checkpoint: CO-2 (2D Array Grid Matrix & Bresenham Algorithms)

**Course Integrated Project:** AIRSTRIKER — 2D Space Shooter Game  
**Target Milestone:** CO-2 Enhancement (Course Outcome 2)  
**Status:** Completed & 100% Fully Verified  

---

## 1. Objectives & Overview
This milestone accomplishes two core curricular enhancements:
1. **Programming Lab (PL / Data Structures):** Transitioning beyond 1D arrays to **2D Array Grid Matrix Data Structures (`AsteroidGridMatrix`)** for procedurally generated multi-lane asteroid obstacle fields in 2D space.
2. **Computer Graphics Lab (CGL):** Implementing **Mid-point Bresenham Line & Circle Drawing Algorithms** for laser targeting reticles, lock-on trajectory lines, plasma shield aura rings, and a real-time radar/minimap scanner.

---

## 2. Theoretical Foundations & Algorithmic Design

### A. 2D Array Grid Matrix (`PL/src/AsteroidGridMatrix.h`, `AsteroidGridMatrix.cpp`)
- **Memory Layout:** Contiguous 2D array matrix: `Asteroid grid[GRID_ROWS][GRID_COLS]` (6 rows $\times$ 8 columns = 48 spatial sector cells).
- **Spatial Lane Division:**
  - $R_0 \dots R_5$: 6 distinct vertical flight corridor lanes spanning playable viewport height ($y \in [50, 530]$).
  - $C_0 \dots C_7$: 8 sequential wave/depth column sectors drifting leftward with wave velocity ($vx = -1.2 - 0.15 \times wave$).
- **Algorithmic Matrix Operations:**
  - **Procedural Sparsity Filter:** Deterministic spatial distribution formula $(r + c + wave) \pmod 3 \neq 0 \land (2r + c) \pmod 5 \neq 0$ ensuring natural gaps for pilot navigation.
  - **2D Bounds Validation:** O(1) Defensive validation $0 \le r < 6 \land 0 \le c < 8$ preventing matrix buffer overflow.
  - **Spatial Collision Detection:** Circle-to-Circle and Point-to-Circle Euclidean distance calculations returning hit indices $(r, c)$.
  - **Cell Lifecycle Transitions:** Intact $\rightarrow$ Damaged $\rightarrow$ Destroyed/Deactivated with score rewards via ALU emulation.

### B. Generalized Bresenham Line Algorithm (`CGL/src/Bresenham.h`, `Bresenham.cpp`)
- **Mathematical Principle:** Pure integer incremental calculation eliminating floating-point division and rounding.
- **Formulation:**
  $$\Delta x = |x_1 - x_0|, \quad \Delta y = |y_1 - y_0|$$
  $$s_x = \text{sgn}(x_1 - x_0), \quad s_y = \text{sgn}(y_1 - y_0)$$
  $$\text{err} = \Delta x - \Delta y$$
- **Step Evaluation:**
  $$\text{If } 2 \cdot \text{err} > -\Delta y \implies \text{err} \gets \text{err} - \Delta y, \quad x \gets x + s_x$$
  $$\text{If } 2 \cdot \text{err} < \Delta x \implies \text{err} \gets \text{err} + \Delta x, \quad y \gets y + s_y$$
- **Coverage:** Exact rasterization across all 8 octants, shallow and steep slopes, arbitrary directions.

### C. Mid-point Bresenham Circle Algorithm (`CGL/src/Bresenham.h`, `Bresenham.cpp`)
- **Mathematical Principle:** Exploits 8-way circle symmetry where computing points in the first octant ($x \le y$) yields 8 coordinates:
  $$(x_c \pm x, y_c \pm y), \quad (x_c \pm y, y_c \pm x)$$
- **Decision Parameter:**
  $$d_0 = 1 - r$$
  $$\text{If } d_k < 0 \implies d_{k+1} = d_k + 2x + 1$$
  $$\text{If } d_k \ge 0 \implies d_{k+1} = d_k + 2(x - y) + 1, \quad y \gets y - 1$$

---

## 3. Game Integration & Features

1. **Procedural Asteroid Obstacle Field (PL):**
   - Active rocky obstacles drifting through space with dynamic cracking fissures when damaged.
   - Laser projectiles collide with asteroids, chipping away health or disintegrating them for bounty points.
   - Spaceship collisions absorb shield energy or cause hull damage.
2. **Dynamic Laser Targeting Reticle (CGL):**
   - Automatically tracks the closest hostile drone ahead of the player.
   - Renders a circular targeting reticle and crosshair spikes using the Bresenham Circle and Line algorithms.
   - Renders a lock-on indicator line from the ship's nose to the target.
3. **Circular Energy Shield Aura Ring (CGL):**
   - Concentric Bresenham circle and dashed pulse ring enclosing the spaceship, fading dynamically as shield is depleted.
4. **Real-time Tactical Radar Minimap (CGL):**
   - Located in the screen corner with circular outer boundary and mid-range concentric ring via Bresenham Circle.
   - Sweep scan line rotating continuously via Bresenham Line.
   - Color-coded entity blips: Cyan (Player), Crimson (Hostile Drones), Amber (Asteroid Grid sectors).

---

## 4. Verification & Test Results

### Automated Unit Test Summary
- **`bin/test_asteroid_grid.exe` (PL CO-2):**
  - Test 1: 2D Array Matrix dimensions (6x8 = 48 cells), index validity, and memory initialization $\rightarrow$ **PASSED**
  - Test 2: Procedural wave generation & multi-lane spread across 6 vertical rows $\rightarrow$ **PASSED**
  - Test 3: Kinematics and left-boundary off-screen deactivation $\rightarrow$ **PASSED**
  - Test 4: 2D spatial collision detection, point/circle hits, and health degradation $\rightarrow$ **PASSED**
- **`bin/test_bresenham.exe` (CGL CO-2):**
  - Test 1: Horizontal, vertical, and diagonal lines rasterized with exact pixel coordinates $\rightarrow$ **PASSED**
  - Test 2: Arbitrary slopes across all 8 octants with correct boundary endpoints $\rightarrow$ **PASSED**
  - Test 3: Mid-point Bresenham Circle 8-way symmetry and integer radius tolerance $\rightarrow$ **PASSED**
  - Test 4: Circular arc angular filtering and quadrant confinement $\rightarrow$ **PASSED**
- **Full Suite Regression:**
  - `test_bullet_pool.exe` $\rightarrow$ **PASSED**
  - `test_enemy_list.exe` $\rightarrow$ **PASSED**
  - `test_alu.exe` $\rightarrow$ **PASSED**
  - `test_transformations.exe` $\rightarrow$ **PASSED**
  - `PSOOPTestRunner` (30/30 Java tests) $\rightarrow$ **PASSED**
