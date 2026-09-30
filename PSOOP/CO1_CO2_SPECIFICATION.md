# Problem Solving Using Object Oriented Programming (Java)
## Academic Outcome Mapping & Specification: CO-1 & CO-2

**Project:** AirStriker 2D Space Shooter  
**Module:** PSOOP (Problem Solving using OOP - Java)  
**Target Syllabus Milestones:** Course Outcome 1 (CO-1) & Course Outcome 2 (CO-2)  
**Status:** 100% Fully Implemented & Verified (77/77 Unit Tests Passing)  

---

## 1. Syllabus Mapping Overview

| Course Outcome | Syllabus Description | Status in AirStriker |
|---|---|:---:|
| **CO-1** | **Develop solution for real world problems using object oriented programming (CO1):**<br/>On the basis of case study create a program to explain the fundamentals of Java (Control statements, Looping, Array), StringBuilder, StringBuffer, Object, Method. | **100% COMPLETE** |
| **CO-2** | **Choose an appropriate programming solution to reduce complexity (CO2):**<br/>Write a program for the Lambda expressions, Inheritance, Polymorphism, Abstract class, Interface, and `static`, `final`, `super` keywords on the basis of case study using Java, with the specification of **why and which mentioned**. | **100% COMPLETE** |

---

## 2. CO-1 Implementation Details (Java Fundamentals)

### 2.1 Control Statements (`if-else` & `switch-case`)
- **Which:**
  - `if`, `else if`, `else`: Used across collision hit tests, health boundaries, shield absorption, and life decrement logic in [`Player.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Player.java) and [`GameEngine.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java).
  - `switch-case`: Implemented in `GameEngine.spawnEnemyByType(String enemyType, double x, double y)` and `GameEngine.getDifficultyMultiplier(String difficulty)`.
- **Why It Reduces Complexity:**
  - `switch-case` replaces long, error-prone `if-else-if` chains when mapping discrete string identifiers (`DRONE`, `FIGHTER`, `BOSS`, `EASY`, `HARD`, `NIGHTMARE`) to factory constructors and multiplier tables. The compiler compiles `switch` on strings into efficient bytecode lookup tables.

### 2.2 Looping (`for`, enhanced `for`, `while`, `do-while`)
- **Which:**
  - **Counted `for` loop:** Used in `GameEngine.spawnWave(int count)` and `GameEngine.findNearestSpawnLane(double targetY)` for deterministic array indexing.
  - **Enhanced `for-each` loop:** Used in `GameEngine.update()` and `resolveCollisions()` to cleanly iterate over active entities.
  - **`while` loop:** Used in `GameEngine.advanceSimulationSteps(int totalFrames)` to step physics frame-by-frame until the frame budget is reached or game-over state occurs.
  - **`do-while` loop:** Used in `GameEngine.spawnEnemiesBatch(int count)` guaranteeing at least one initial spawn cycle occurs before testing loop boundaries.
- **Why It Reduces Complexity:**
  - Selecting the appropriate looping structure minimizes off-by-one errors. `while` allows early state-dependent exits without nested break flags, while `do-while` cleanly expresses guaranteed-initialization logic.

### 2.3 Array (1D Contiguous Memory)
- **Which:**
  - `private static final double[] SPAWN_LANES = { 100.0, 200.0, 300.0, 400.0, 500.0 };` in [`GameEngine.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java#L78).
  - Method `findNearestSpawnLane(double targetY)` performs linear traversal over the array.
- **Why It Reduces Complexity:**
  - Predefined spatial lanes provide $O(1)$ random-access indexed placement for procedural enemy formations without dynamic heap allocation during the 60 FPS combat loop.

### 2.4 StringBuilder & StringBuffer
- **Which:**
  - **`StringBuilder`:** Implemented in [`GameState.getHudDisplayText()`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/GameState.java#L80-L88).
  - **`StringBuffer`:** Implemented in [`GameEngine.getMissionSummary()`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java#L273-L280).
- **Why It Reduces Complexity:**
  - Every 16.6ms (60 FPS), the HUD telemetry string is generated. Repeated String concatenation (`+`) produces dozens of immutable String garbage objects per second. `StringBuilder` constructs the text inside a single reusable char array buffer.
  - `StringBuffer` provides thread-safe string mutation for mission summary reporting.

### 2.5 Object & Method
- **Which:**
  - Object modeling across domain classes: [`Character`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Character.java), [`Player`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Player.java), [`Enemy`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Enemy.java), [`Bullet`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Bullet.java), [`GameState`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/GameState.java), and [`GameEngine`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java).
  - Methods include constructor chaining (`this(...)`), bounded mutators, and behavior methods (`takeDamage()`, `heal()`, `move()`, `fireBullet()`, `resolveCollisions()`).
- **Why It Reduces Complexity:**
  - Strict encapsulation packages state variables with boundary validation logic, protecting domain models against corrupted coordinates or negative health.

---

## 3. CO-2 Implementation Details (Reducing Complexity via OOP)

### 3.1 Lambda Expressions (Built-in & Custom @FunctionalInterface)
- **Which:**
  1. **Built-in `Comparator` Lambda:**  
     `enemies.sort((e1, e2) -> Double.compare(e1.getX(), e2.getX()));` in [`GameEngine.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java#L247)
  2. **Built-in `Predicate` Lambda:**  
     `bullets.removeIf(b -> !b.isActive());` and `enemies.removeIf(e -> !e.isActive());` in [`GameEngine.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/service/GameEngine.java#L198-L206)
  3. **Custom `@FunctionalInterface` Consumer (`EnemyAction`):**  
     Declared in [`EnemyAction.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/EnemyAction.java) and consumed in `GameEngine.forEachActiveEnemy(EnemyAction action)`.
  4. **Custom `@FunctionalInterface` Return Formula (`ScoreCalculator`):**  
     Declared in [`ScoreCalculator.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/ScoreCalculator.java) and consumed in `GameEngine.calculateComboBonus(int comboStreak, ScoreCalculator calculator)`.
- **Why It Reduces Complexity:**
  - Completely eliminates boilerplate anonymous inner classes. Higher-order functions allow external modules to pass ad-hoc filtering, EMP area-of-effect shocks, and dynamic scoring rules directly as clean, readable 1-line lambdas.

### 3.2 Inheritance (Multilevel Class Hierarchy)
- **Which:**
  - Hierarchy:
    ```
    Character (Abstract Root)
      ├── Player
      └── Enemy
            ├── Drone
            ├── Fighter
            └── Boss
    ```
- **Why It Reduces Complexity:**
  - Reuses foundational coordinate math, speed, health clamping, and active status in `Character`. Specializations only declare what is unique to them (`Player` adds shields; `Drone` adds oscillation; `Fighter` adds evasion sweep; `Boss` adds armor & enrage phase).

### 3.3 Polymorphism (Compile-time & Runtime)
- **Which:**
  - **Compile-time Polymorphism (Method Overloading):**
    - Overloaded constructors in all model classes.
    - `Player.move()` vs `Player.move(double dx, double dy, double minX, double maxX, double minY, double maxY)`.
  - **Runtime Polymorphism (Method Overriding & Dynamic Dispatch):**
    - `move()` is overridden across `Player`, `Enemy`, `Drone`, `Fighter`, and `Boss`.
    - `takeDamage()` is overridden in `Player` (shield interception) and `Boss` (armor mitigation & phase transition).
- **Why It Reduces Complexity:**
  - The game loop iterates over a single collection `List<Enemy> enemies` and simply calls `e.move()`. Dynamic method dispatch routes to the correct subclass algorithm automatically, eliminating massive `if (enemy.type == "DRONE")` branching in the engine.

### 3.4 Abstract Class
- **Which:**
  - `public abstract class Character implements Movable` in [`Character.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Character.java).
  - Declares the abstract method: `public abstract void move();`.
- **Why It Reduces Complexity:**
  - Prevents erroneous instantiation of a generic "Character" (a character in space must always be a concrete vessel). It guarantees that any future developer adding a new ship *must* provide a concrete `move()` implementation.

### 3.5 Interface
- **Which:**
  - `public interface Movable` in [`Movable.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Movable.java).
- **Why It Reduces Complexity:**
  - Decouples movement behavior from concrete class hierarchies. Any entity (player ship, drone, asteroid, missile) can implement `Movable` without forcing unrelated objects into a single inheritance tree (supporting Interface Segregation).

### 3.6 Keyword: `static`
- **Which:**
  - Static constants: `GameEngine.SCREEN_MIN_X`, `SCREEN_MAX_X`, `SCREEN_MIN_Y`, `SCREEN_MAX_Y`.
  - Static helper methods: `GameEngine.isWithinBounds(double x, double y)` and `GameEngine.clamp(...)`.
- **Why It Reduces Complexity:**
  - Centralizes immutable engine dimensions and universal coordinate utility algorithms in a single location, accessible anywhere without instantiating an engine object.

### 3.7 Keyword: `final`
- **Which:**
  1. **`final` Methods:** Declared in [`Character.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Character.java) for `isAlive()`, `getId()`, `getMaxHealth()`, and `isActive()`.
  2. **`final` Class:** [`Bullet.java`](file:///c:/Users/lenovo/OneDrive/Desktop/AirStrikeeee/airstrikeenhance/PSOOP/src/model/Bullet.java) is declared as `public final class Bullet`.
  3. **`final` Constants:** Static final boundaries.
  4. **`final` Parameters:** Method arguments across `Player`, `Bullet`, `Enemy`, and `GameEngine`.
- **Why It Reduces Complexity:**
  - Enforces invariant safety. Marking `isAlive()` as `final` guarantees that subclasses cannot override and corrupt life-or-death logic. Marking `Bullet` as `final` prevents unintended inheritance of lightweight projectile value objects. Final parameters protect inputs from accidental re-assignment inside complex math routines.

### 3.8 Keyword: `super`
- **Which:**
  - Super constructor calls: `super(id, name, ...)` in `Player`, `Enemy`, `Drone`, `Fighter`, and `Boss`.
  - Super method calls: `super.takeDamage(remainingDamage)` in `Player.java` and `super.takeDamage(effectiveDamage)` in `Boss.java`.
- **Why It Reduces Complexity:**
  - Child classes don't re-implement base initialization or base damage reduction logic; they delegate to `super`, preventing code duplication and ensuring base invariants are always initialized first.

---

## 4. Test Verification Summary (77/77 Unit Tests)

Run test command:
```bash
cmd.exe /c run_psoop_tests.bat
```

| Test Suite ID | Test Description | Assertions | Status |
|---|---|:---:|:---:|
| **Test 1** | Character Encapsulation, Abstraction & Final Methods (CO-1 & CO-2) | 10 | **PASSED** |
| **Test 2** | Player Subclass, Shield Absorption & Super Calls (CO-2) | 9 | **PASSED** |
| **Test 3** | Enemy, Final Bullet Class Dynamics (CO-1 & CO-2) | 5 | **PASSED** |
| **Test 4** | Control Statements: Switch-Case Branching & Multipliers (CO-1) | 8 | **PASSED** |
| **Test 5** | Looping Constructs: For, Enhanced-For, While & Do-While (CO-1) | 4 | **PASSED** |
| **Test 6** | Array, StringBuilder & StringBuffer (CO-1) | 5 | **PASSED** |
| **Test 7** | Multilevel Inheritance Hierarchy: Drone, Fighter, Boss (CO-2) | 22 | **PASSED** |
| **Test 8** | Lambda Expressions: Built-in & Custom @FunctionalInterface (CO-2) | 6 | **PASSED** |
| **Test 9** | GameEngine Simulation & Polymorphic Collision Integration | 8 | **PASSED** |
| **TOTAL** | **Comprehensive Automated Verification** | **77 / 77** | **100% PASSED** |
