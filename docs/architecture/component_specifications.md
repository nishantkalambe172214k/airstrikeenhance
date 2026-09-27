# AIRSTRIKER — SUBSYSTEM COMPONENT SPECIFICATIONS

```
================================================================================
               COLLEGE INTEGRATED ENGINEERING CAPSTONE PROJECT
                     SUBSYSTEM COMPONENT SPECIFICATIONS
================================================================================
```

This document details the low-level technical specifications, data contracts, API interfaces, preconditions, postconditions, and operational invariants for each of the four integrated subsystems.

---

## 1. Programming Lab (PL) — Data Structures Subsystem

- **Primary Source Files:** [`PL/src/BulletPoolArray.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/src/BulletPoolArray.h), [`PL/src/BulletPoolArray.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/PL/src/BulletPoolArray.cpp)
- **Language / Standard:** Pure C++ (Standard C++11)
- **Primary Data Structure:** Fixed-capacity contiguous static array pool

### 1.1 Data Structures Definition

```cpp
const int MAX_BULLETS = 64;

struct Bullet {
    int bulletID;    // Unique monotonic identifier
    float x;         // World X coordinate [0.0, 800.0]
    float y;         // World Y coordinate [0.0, 600.0]
    float vx;        // Horizontal velocity component
    float vy;        // Vertical velocity component
    int damage;      // Damage dealt upon impact (default: 25)
    bool active;     // Slot allocation flag (true = in flight, false = free)
};
```

### 1.2 Class Invariants
1. **Capacity Invariant:** `0 <= activeBullets <= MAX_BULLETS (64)`.
2. **ID Monotonicity:** `nextBulletID` strictly increases upon each successful allocation.
3. **Contiguity Invariant:** All 64 `Bullet` instances reside in contiguous heap/BSS/stack memory without pointer indirection.
4. **Clean Reclamation Invariant:** Deactivated slots (`active == false`) must be available for immediate reallocation on the subsequent `fireBullet` call.

### 1.3 Method Specifications

#### `int fireBullet(float x, float y, float vx, float vy, int damage)`
- **Precondition:** `damage > 0`.
- **Operation:** Scans `bullets[0..63]` sequentially for the first slot where `active == false`.
- **Postcondition:** If found, initializes the slot fields, sets `active = true`, increments `totalFiredCount`, and returns `bulletID`. If all 64 slots are active, returns `-1`.
- **Complexity:** Best-case $O(1)$, Worst-case $O(N)$, Space $O(1)$.

#### `void updateBullets(float minX, float maxX, float minY, float maxY)`
- **Precondition:** Valid rectangular boundary coordinates where `minX < maxX` and `minY < maxY`.
- **Operation:** Iterates from $i = 0$ to $63$. For each slot with `active == true`:
  - $x \leftarrow x + vx$
  - $y \leftarrow y + vy$
  - If $x < \text{minX}$ or $x > \text{maxX}$ or $y < \text{minY}$ or $y > \text{maxY}$, sets `active = false`.
- **Postcondition:** Coordinates updated; out-of-bounds projectiles recycled.
- **Complexity:** $O(N)$ strictly ($N=64$).

#### `int searchBullet(int bulletID)`
- **Precondition:** `bulletID > 0`.
- **Operation:** Executes linear search through `bullets[0..63]` checking for matching `bulletID` and `active == true`.
- **Postcondition:** Returns array index $[0, 63]$ if located; returns `-1` if not found.
- **Complexity:** $O(N)$ linear search.

---

## 2. Problem Solving using OOP (PSOOP) — Enterprise Domain Subsystem

- **Primary Source Files:** `PSOOP/src/model/*.java`, `PSOOP/src/service/GameEngine.java`
- **Language / Standard:** Java (JDK 17+)
- **Architecture Pattern:** Model-Service Separation

### 2.1 Class Contract: `Character` (Base Class)
```java
public class Character {
    private int id;
    private String name;
    private double x;
    private double y;
    private int health;
    private int maxHealth;
    private double speed;
    private boolean active;
    // Getters, validated setters, kinematics, boundary clamps
}
```
- **Encapsulation Rules:** Direct access to instance fields is prohibited.
- **Validation Rules:**
  - `name`: Must not be null or blank; length between $2$ and $25$ characters.
  - `health`: Clamped to range $[0, \text{maxHealth}]$.
  - `speed`: Strictly non-negative ($\ge 0.0$).

### 2.2 Class Contract: `Player` (Subclass)
```java
public class Player extends Character {
    private int shield;
    private int maxShield;
    private int score;
    private int lives;
    // Custom takeDamage() with shield absorption
}
```
- **Shield Absorption Logic (Method Overriding):**
  ```java
  @Override
  public void takeDamage(int amount) {
      if (shield >= amount) {
          shield -= amount;
      } else {
          int residual = amount - shield;
          shield = 0;
          super.takeDamage(residual);
      }
  }
  ```

### 2.3 Class Contract: `Enemy` (Subclass)
```java
public class Enemy extends Character {
    private int scoreValue;
    private int collisionDamage;
    private String droneType;
}
```
- **Drone Attributes:** Default drone carries $25$ health, deals $25$ collision damage, awards $250$ score bounty.

### 2.4 Service Contract: `GameEngine`
- **Role:** Central domain orchestrator managing player, enemy lists, bullet lists, collision sweeps, and wave progression.
- **Collision Sweep Algorithm (AABB):**
  $$\text{Overlap} \iff |X_1 - X_2| < \frac{W_1 + W_2}{2} \ \land \ |Y_1 - Y_2| < \frac{H_1 + H_2}{2}$$

---

## 3. Computer Organization & Architecture (COA) Subsystem

- **Primary Source Files:** [`COA/ALU/ALU80386.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALU/ALU80386.h), [`COA/ALU/ALU80386.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALU/ALU80386.cpp), `COA/ALP/*.asm`
- **Curriculum Architecture Models:**
  1. **YASMIN CPU-OS Simulator (Edge Hill University - Besim Mustafa):** 32 General Purpose Registers (`R00`-`R31`), Educational RISC instruction set.
  2. **Intel 80386 Architecture:** 32-bit registers (`EAX`, `EBX`, `ECX`, `EDX`) and 32-bit `EFLAGS` register.

### 3.1 Simulated 80386 Register File & Flag Map
```cpp
enum EFlagsMask : uint32_t {
    FLAG_CF = (1u << 0),   // Bit 0: Carry Flag
    FLAG_PF = (1u << 2),   // Bit 2: Parity Flag (Least Significant Byte)
    FLAG_AF = (1u << 4),   // Bit 4: Auxiliary Carry Flag (Nibble carry)
    FLAG_ZF = (1u << 6),   // Bit 6: Zero Flag
    FLAG_SF = (1u << 7),   // Bit 7: Sign Flag
    FLAG_OF = (1u << 11)   // Bit 11: Overflow Flag
};

struct Registers80386 {
    uint32_t EAX;     // Accumulator (Score accumulator / Health accumulator)
    uint32_t EBX;     // Base register
    uint32_t ECX;     // Count register
    uint32_t EDX;     // Data register (Score bounty / Incoming damage)
    uint32_t EFLAGS;  // Condition Code & Status Register
};
```

### 3.2 Micro-Operation Specifications

#### `executeADD(bool verbose)`
- **Operation:** Computes $\text{Result} = \text{EAX} + \text{EDX}$.
- **Flag Updates:**
  - $\text{CF} = (\text{Result} < \text{EAX})$
  - $\text{ZF} = (\text{Result} == 0)$
  - $\text{SF} = (\text{Result} \ \& \ \text{0x80000000}) \neq 0$
  - $\text{OF} = (\sim(\text{EAX} \oplus \text{EDX}) \ \& \ (\text{EAX} \oplus \text{Result}) \ \& \ \text{0x80000000}) \neq 0$
  - $\text{PF} = \text{Even parity of least significant 8 bits}$
- **Game Mapping:** Player destroys enemy $\to$ Adds bounty to score.

#### `executeSUB(bool verbose)`
- **Operation:** Computes $\text{Result} = \text{EAX} - \text{EDX}$.
- **Flag Updates:**
  - $\text{CF} = (\text{EAX} < \text{EDX})$ (Borrow occurred)
  - $\text{ZF} = (\text{Result} == 0)$ (Exact zero $\to$ Fatal hull breach)
  - $\text{SF} = (\text{Result} \ \& \ \text{0x80000000}) \neq 0$ (Negative $\to$ Overkill fatal hull breach)
  - $\text{OF} = ((\text{EAX} \oplus \text{EDX}) \ \& \ (\text{EAX} \oplus \text{Result}) \ \& \ \text{0x80000000}) \neq 0$
- **Game Mapping:** Ship sustains damage $\to$ Deducts health and checks `ZF`/`SF` for `GAME_OVER`.

---

## 4. Computer Graphics Lab (CGL) Subsystem

- **Primary Source Files:** [`CGL/src/Renderer.h`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/Renderer.h), [`CGL/src/Renderer.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/Renderer.cpp), [`CGL/src/MainGame.cpp`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/CGL/src/MainGame.cpp)
- **Graphics API:** OpenGL Utility Toolkit (GLUT / FreeGLUT) + Core OpenGL 1.1 / 2.0
- **Rendering Paradigm:** Procedural Geometric Primitives (No external image assets)

### 4.1 OpenGL State Machine Configuration
```cpp
glViewport(0, 0, width, height);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluOrtho2D(0.0, width, 0.0, height);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

### 4.2 Procedural Primitive Breakdown
| Mesh Component | OpenGL Primitive | Color Vector (RGBA) | Visual Description |
|---|---|---|---|
| **Starfield** | `GL_POINTS` | `(1.0, 1.0, 1.0, brightness)` | 90 procedural stars over 3 parallax velocity layers |
| **Ship Fuselage** | `GL_TRIANGLES` | `(0.1, 0.6, 0.95)` | Streamlined aerodynamic interceptor nose & body |
| **Ship Wings** | `GL_QUADS` | `(0.05, 0.45, 0.8)` | Forward-swept delta wings with dark navy accents |
| **Cockpit Canopy** | `GL_POLYGON` | `(0.8, 0.95, 1.0, 0.85)` | Prismatic diamond cockpit glass with alpha transparency |
| **Plasma Thruster** | `GL_TRIANGLES` | `(1.0, 0.5 + 0.3 \sin(t), 0.0)` | Animated flickering flame plume modulated by sine wave |
| **Alien Drone** | `GL_TRIANGLES` & `GL_QUADS` | `(0.9, 0.15, 0.2)` | Aggressive crimson wedge fighter with central sensor eye |
| **Laser Bolt Core** | `GL_QUADS` | `(1.0, 1.0, 1.0)` | Intense white interior projectile core |
| **Laser Bolt Aura** | `GL_QUADS` | `(0.0, 0.8, 1.0, 0.4)` | Cyan luminous outer energy glow |
| **Arcade HUD** | `glutBitmapCharacter` | Dynamic (Green/Yellow/Red) | Real-time score, wave, health bar, and CPU flag register display |
