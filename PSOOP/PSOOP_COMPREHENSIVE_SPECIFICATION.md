# Problem Solving Using Object Oriented Programming (Java)
## Comprehensive Academic Outcome Specification: Exception Handling, Java Swing, Packages & Collections

**Project:** AirStriker 2D Space Shooter  
**Module:** PSOOP (Problem Solving using OOP - Java / JDK 26)  
**Status:** 100% Fully Implemented, Integrated & Verified (146/146 Automated Unit Tests Passing)  

---

## 1. Executive Summary & Verification Matrix

The AirStriker PSOOP subsystem has been comprehensively engineered to integrate all four core syllabus milestones:

| Academic Topic | Primary Architectural Role in AirStriker | Classes / Files | Tests Passed |
|---|---|---|:---:|
| **1. Exception Handling** | Custom checked & unchecked domain exceptions, precondition defense, weapon heat limit lockout, shield exhaustion, and `try-catch-finally` simulation resilience. | `GameException.java`<br/>`WeaponOverheatException.java`<br/>`ShieldDepletedException.java`<br/>`InvalidEntityStateException.java`<br/>`GameOverException.java`<br/>`Player.java`<br/>`Character.java`<br/>`GameEngine.java` | 19 / 19 |
| **2. Java Swing** | Complete 60 FPS desktop arcade game with double-buffered canvas, multi-key flight controls, glassmorphic HUD, particle effects, and modal dialogs. | `GamePanel.java`<br/>`GameWindow.java`<br/>`LeaderboardDialog.java`<br/>`SwingGameLauncher.java` | 12 / 12 |
| **3. Packages** | Modular architectural separation into 5 distinct, decoupled packages (`model`, `service`, `exception`, `ui`, `test`) with strict access control and clean imports. | `package model;`<br/>`package service;`<br/>`package exception;`<br/>`package ui;`<br/>`package test;` | 15 / 15 |
| **4. Collections** | Comprehensive use of Java Collections Framework (`List`, `ArrayList`, `Map`, `HashMap`, `Set`, `HashSet`, `Queue`, `LinkedList`, `Deque`, `ArrayDeque`, `Iterator`, `Collections.sort()`). | `GameEngine.java`<br/>`LeaderboardService.java`<br/>`GamePanel.java`<br/>`WaveSquad.java`<br/>`PilotProfile.java` | 23 / 23 |
| **CO-1 & CO-2 Baseline** | Control statements, loops, 1D array, StringBuilder, StringBuffer, inheritance, encapsulation, and lambda expressions. | `Character.java`, `Player.java`, `Enemy.java`, `Drone.java`, `Fighter.java`, `Boss.java`, `Bullet.java`, etc. | 40 / 40 |
| **CO-3 Combat System** | Abstract classes, `Movable` interface, dynamic polymorphism (`move()` variations across Drone, Fighter, Boss, Player), multi-tier deflector shield absorption, hull damage delegation, bounded combat kinematics, and bounty scoring. | `Player.java`<br/>`Movable.java`<br/>`Character.java`<br/>`Enemy.java`<br/>`Drone.java`<br/>`Fighter.java`<br/>`Boss.java` | 37 / 37 |
| **TOTAL VERIFICATION** | **Master Automated Test Suite (`PSOOPTestRunner`)** | **All 17 Source Files** | **146 / 146 (100% PASSED)** |

---

## 2. Topic 1: Exception Handling Implementation

### 2.1 Exception Hierarchy Architecture
- `java.lang.Throwable`
  - `java.lang.Exception` (Checked)
    - **`exception.GameException`** (Base checked game exception with `errorCode`)
      - **`exception.WeaponOverheatException`** (Thrown when weapon heat exceeds 100% or ammo is exhausted)
      - **`exception.ShieldDepletedException`** (Thrown when shield absorption is attempted with 0 shield)
      - **`exception.GameOverException`** (Thrown when player actions are attempted after mission failure)
  - `java.lang.RuntimeException` (Unchecked)
    - **`exception.InvalidEntityStateException`** (Thrown on corrupted invariants, NaN coordinates, or negative bounds)

### 2.2 Exact Line Numbers & Functional Purpose

| File | Line Numbers | Construct | Purpose in Project |
|---|---|---|---|
| [`GameException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/GameException.java) | Lines 7–30 | `class GameException extends Exception` | Root checked exception carrying standardized machine-readable error codes (`getErrorCode()`) and causes. |
| [`WeaponOverheatException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/WeaponOverheatException.java) | Lines 7–23 | `class WeaponOverheatException extends GameException` | Thrown by cannon firing routines when heat exceeds `MAX_WEAPON_HEAT` (100.0). Stores `currentHeat` and `maxHeat`. |
| [`ShieldDepletedException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/ShieldDepletedException.java) | Lines 7–23 | `class ShieldDepletedException extends GameException` | Thrown when deflector shield absorption is commanded on depleted generator (`shield == 0`). Stores `requestedAbsorption`. |
| [`InvalidEntityStateException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/InvalidEntityStateException.java) | Lines 7–20 | `class InvalidEntityStateException extends RuntimeException` | Unchecked domain exception thrown when an entity is instantiated or mutated with illegal invariants (negative HP, NaN coordinates). |
| [`GameOverException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/GameOverException.java) | Lines 7–23 | `class GameOverException extends GameException` | Thrown when a player attempts combat maneuvers after total hull loss and zero remaining lives. Stores `finalScore` and `finalWave`. |
| [`Player.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/Player.java) | Lines 115–130 | `public Bullet fireWeaponChecked(...) throws WeaponOverheatException, GameOverException` | Validates mission state (`GameOverException`) and weapon heat threshold (`WeaponOverheatException`), firing only if heat conditions allow. |
| [`Player.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/Player.java) | Lines 132–140 | `public int absorbShieldChecked(...) throws ShieldDepletedException` | Throws `ShieldDepletedException` if shield is 0, protecting hull callers from silent absorption failures. |
| [`Character.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/Character.java) | Lines 98–116 | `public void validateState() throws InvalidEntityStateException` | Asserts that ID is non-negative, callsign is non-empty, health is bounded in `[0, maxHealth]`, and coordinates are not `NaN`. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 130–138 | `public Enemy spawnEnemyByTypeValidated(...) throws InvalidEntityStateException` | Throws `InvalidEntityStateException` when enemy spawn coordinates violate spatial combat sector boundaries. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 248–270 | `public int safeAdvanceSimulation(...) throws GameException` | Demonstrates full `try`, `catch`, `finally` block: catches runtime faults, logs telemetry, and guarantees `SIM_SYNC` status cleanup in `finally`. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 275–286 | `public Bullet fireBulletChecked(...) throws WeaponOverheatException, GameOverException` | High-level engine method declaring and propagating firing exceptions to calling GUI or test clients. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 288–301 | `public Bullet fireBullet()` | Graceful fallback method with `try-catch` catching `WeaponOverheatException` and `GameOverException`, preserving legacy backward compatibility. |
| [`LeaderboardService.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/LeaderboardService.java) | Lines 41–94 | `public void loadLeaderboard() throws GameException` | Uses `try`, `catch(IOException \| NumberFormatException)`, and explicit `finally` block to guarantee closing of `BufferedReader`. |
| [`LeaderboardService.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/LeaderboardService.java) | Lines 99–114 | `public void saveLeaderboard() throws GameException` | Demonstrates modern Java `try-with-resources` (`PrintWriter`) and handles I/O exceptions. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 142–156 | `try { engine.fireBulletChecked(); } catch (WeaponOverheatException woe) { ... }` | Catches `WeaponOverheatException` on Spacebar input to trigger the HUD flashing red alert banner `! OVERHEAT LOCKOUT !`. |

---

## 3. Topic 2: Java Swing Desktop Application

### 3.1 GUI Architectural Components
1. **`GameWindow` (`JFrame`):** Desktop window (850×620), menu bar with keyboard shortcuts (Ctrl+N, Ctrl+H, P, R, F1), custom title and dialog triggers.
2. **`GamePanel` (`JPanel`):** Custom 2D double-buffered canvas running at 60 FPS via `javax.swing.Timer(16, this)`:
   - Parallax twinkling starfield (`List<Star>`).
   - Procedural Player StarViper with animated pulsating thrusters and deflector shield bubble.
   - Polymorphic enemy graphics: Crimson diamond Drones, Violet swept-wing Fighters, Obsidian Dreadnought Boss with red enrage aura.
   - Glowing laser bolts and particle explosion sparks (`List<Particle>`).
   - Top Glassmorphism HUD Bar: Callsign, Gold score counter, Health bar (green-yellow-red), Shield bar (cyan), Weapon Heat gauge with overheat lockout alert, and Lives silhouettes.
   - Bottom Combat Telemetry Bar: Rolling real-time event ticker from `engine.getCombatEventLog()`.
   - Overlays: Mission Paused and Mission Failed / Game Over screen with final statistics.
3. **`LeaderboardDialog` (`JDialog`):** Modal dialog with `JTable` and `DefaultTableModel` styled with a dark sci-fi palette displaying ranked high scores.
4. **`SwingGameLauncher`:** Standalone entry point launching on the Swing Event Dispatch Thread (`SwingUtilities.invokeLater`).

### 3.2 Exact Line Numbers & Functional Purpose

| File | Line Numbers | Construct | Purpose in Project |
|---|---|---|---|
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 45–78 | `public class GamePanel extends JPanel implements ActionListener, KeyListener` | Master Swing game canvas handling 60 FPS animation loop, user keyboard input, and double-buffered graphics. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 110–113 | `this.gameTimer = new Timer(16, this); gameTimer.start();` | Swing `Timer` generating periodic tick events every 16ms (~60 FPS) to advance physics and invoke `repaint()`. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 208–245 | `protected void paintComponent(Graphics g)` | Overridden Swing paint method: configures `Graphics2D` antialiasing hints and paints stars, particles, ships, lasers, and HUD. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 375–445 | `private void drawHUD(Graphics2D g2)` | Draws translucent top glassmorphism HUD bar with dynamic health, shield, weapon heat gauges, and overheat warning. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 475–515 | `private void drawGameOverOverlay(Graphics2D g2)` | Renders modal game-over scrim with final score, wave reached, and restart instructions. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 630–636 | `public void stopTimer()` | Cleanly halts the Swing `Timer` thread during testing or shutdown to prevent background thread leaks. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 640–660 | `keyPressed(...)` and `keyReleased(...)` | Swing `KeyListener` implementation capturing WASD, Arrow keys, Spacebar, P (Pause), and R (Restart). |
| [`LeaderboardDialog.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/LeaderboardDialog.java) | Lines 27–85 | `public class LeaderboardDialog extends JDialog` | Modal dialog housing `JTable`, `DefaultTableModel`, `JScrollPane`, and `JButton` for pilot hall of fame. |
| [`LeaderboardDialog.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/LeaderboardDialog.java) | Lines 90–115 | `public void reloadData()` | Queries `LeaderboardService` and dynamically populates table rows with ranked pilot records. |
| [`GameWindow.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GameWindow.java) | Lines 19–45 | `public class GameWindow extends JFrame` | Top-level Swing desktop window encapsulating `GamePanel` and `JMenuBar`. |
| [`GameWindow.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GameWindow.java) | Lines 47–98 | `private JMenuBar createMenuBar()` | Creates Swing menu bar with Mission, Telemetry, and Help menus, bound to keyboard accelerators (`KeyStroke`). |
| [`GameWindow.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GameWindow.java) | Lines 100–145 | `JOptionPane` dialog dialogs | Uses `JOptionPane.showInputDialog` for pilot name entry and `JOptionPane.showMessageDialog` for flight manual. |
| [`SwingGameLauncher.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/SwingGameLauncher.java) | Lines 12–45 | `SwingUtilities.invokeLater(() -> ...)` | Guarantees thread-safe initialization of Swing GUI components exclusively on the Event Dispatch Thread (EDT). |

---

## 4. Topic 3: Packages & Modular Architecture

### 4.1 Package Layout
The PSOOP codebase is organized into five modular packages following standard Java package conventions:

```
PSOOP/src/
├── model/           # Domain Entities & Value Models
│   ├── Character.java
│   ├── Player.java
│   ├── Enemy.java
│   ├── Drone.java
│   ├── Fighter.java
│   ├── Boss.java
│   ├── Bullet.java
│   ├── GameState.java
│   ├── Movable.java
│   ├── EnemyAction.java
│   ├── ScoreCalculator.java
│   ├── WaveSquad.java
│   └── PilotProfile.java
│
├── service/         # Business Logic, Orchestration & Persistence
│   ├── GameEngine.java
│   ├── LeaderboardService.java
│   └── PSOOPBridgeServer.java
│
├── exception/       # Domain Exception Hierarchy
│   ├── GameException.java
│   ├── WeaponOverheatException.java
│   ├── ShieldDepletedException.java
│   ├── InvalidEntityStateException.java
│   └── GameOverException.java
│
├── ui/              # Java Swing Desktop GUI Application
│   ├── GamePanel.java
│   ├── GameWindow.java
│   ├── LeaderboardDialog.java
│   └── SwingGameLauncher.java
│
└── test/            # Verification Suite
    └── PSOOPTestRunner.java
```

### 4.2 Exact Line Numbers & Functional Purpose

| File | Line Number | Package Statement | Purpose in Project |
|---|---|---|---|
| [`Character.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/Character.java) | Line 1 | `package model;` | Encapsulates fundamental entity attributes and movement contracts. |
| [`Player.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/Player.java) | Line 1 | `package model;` | Encapsulates pilot vessel state (shield, weapon heat, ammo, lives, score). |
| [`WaveSquad.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/WaveSquad.java) | Line 1 | `package model;` | Encapsulates squadron wave configuration data for Queue processing. |
| [`PilotProfile.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/PilotProfile.java) | Line 1 | `package model;` | Encapsulates leaderboard records and natural sort comparison. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Line 1 | `package service;` | Central business orchestrator executing physics, wave queues, and collisions. |
| [`LeaderboardService.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/LeaderboardService.java) | Line 1 | `package service;` | Service managing CSV persistence and in-memory pilot score indexing. |
| [`GameException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/GameException.java) | Line 1 | `package exception;` | Root checked exception for domain error handling across all packages. |
| [`WeaponOverheatException.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/exception/WeaponOverheatException.java) | Line 1 | `package exception;` | Domain exception isolating cannon thermal failure logic. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Line 1 | `package ui;` | Swing presentation tier rendering engine state and handling user events. |
| [`GameWindow.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GameWindow.java) | Line 1 | `package ui;` | Swing presentation tier managing window lifecycle and menu commands. |
| [`PSOOPTestRunner.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/test/PSOOPTestRunner.java) | Line 1 | `package test;` | Standalone test package decoupled from production game logic. |

---

## 5. Topic 4: Java Collections Framework

### 5.1 Collections Employed & Concrete Types

1. **`List` / `ArrayList`:**
   - `List<Bullet> bullets` in `GameEngine.java`: Dynamic tracking of projectiles.
   - `List<Enemy> enemies` in `GameEngine.java`: Dynamic polymorphic hostile list.
   - `List<PilotProfile> profiles` in `LeaderboardService.java`: Pilot records.
   - `List<Star>` and `List<Particle>` in `GamePanel.java`: Background stars and sparks.
2. **`Map` / `HashMap`:**
   - `Map<Integer, Enemy> enemyMap` in `GameEngine.java`: $O(1)$ constant-time lookup by enemy ID.
   - `Map<String, Integer> highScoresByCallsign` in `LeaderboardService.java`: Fast score lookup by pilot callsign.
   - `Map<Integer, PilotProfile> profilesById` in `LeaderboardService.java`: Profile lookup by ID.
3. **`Set` / `HashSet`:**
   - `Set<Integer> destroyedEnemyIds` in `GameEngine.java`: Guarantees unique tracking of destroyed hostiles without duplicate scoring.
   - `Set<Integer> pressedKeys` in `GamePanel.java`: Tracks currently pressed keycodes for responsive multi-key diagonal flight.
4. **`Queue` / `LinkedList`:**
   - `Queue<WaveSquad> waveQueue` in `GameEngine.java`: First-In-First-Out (FIFO) pipeline for scheduling procedural hostile reinforcements.
5. **`Deque` / `ArrayDeque`:**
   - `Deque<String> combatEventLog` in `GameEngine.java`: Double-ended queue storing rolling combat event telemetry (peek last for HUD ticker, poll first when reaching max capacity).
6. **`Iterator`:**
   - Safe traversal and concurrent modification prevention during bullet and enemy cleanup in `GameEngine.java` (`bulletIt.remove()`, `enemyIt.remove()`) and `GamePanel.java` (`particles.iterator()`).
7. **`Collections` Utility Methods:**
   - `Collections.sort(profiles)`: Natural descending order sorting via `Comparable<PilotProfile>`.
   - `Collections.unmodifiableList(...)` and `Collections.unmodifiableMap(...)`: Enforces immutability on exposed collections to prevent external state corruption.

### 5.2 Exact Line Numbers & Functional Purpose

| File | Line Numbers | Collection Construct | Purpose in Project |
|---|---|---|---|
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 38–44 | `Map<Integer, Enemy> enemyMap = new HashMap<>()` | Provides $O(1)$ constant-time lookup of active hostile ships by numeric entity ID. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 40–44 | `Set<Integer> destroyedEnemyIds = new HashSet<>()` | Enforces mathematical set uniqueness for defeated enemies, preventing double-kill bugs. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 41–44 | `Queue<WaveSquad> waveQueue = new LinkedList<>()` | Implements FIFO reinforcement pipeline for upcoming procedural waves. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 42–44 | `Deque<String> combatEventLog = new ArrayDeque<>()` | Maintains fixed-capacity rolling combat log displaying telemetry in Swing HUD. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 182–215 | `enqueueWaveSquad(...)` & `pollNextSquad()` | Uses `Queue.offer()` and `Queue.poll()` to manage incoming squadron deployments. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 311–318 | `Iterator<Bullet> bulletIt = bullets.iterator()` | Iterates through bullets and uses `bulletIt.remove()` for safe deletion without `ConcurrentModificationException`. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 326–336 | `Iterator<Enemy> enemyIt = enemies.iterator()` | Iterates through enemies, removing dead ships and pruning them from `enemyMap`. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 380–385 | `destroyedEnemyIds.add(e.getId())` | Inserts defeated enemy ID into `Set` upon confirmed collision impact. |
| [`GameEngine.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/GameEngine.java) | Lines 422–446 | `Collections.unmodifiableMap(...)` & `unmodifiableSet(...)` | Protects internal engine collections from outside mutation. |
| [`LeaderboardService.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/LeaderboardService.java) | Lines 29–34 | `List<PilotProfile>`, `Map<String, Integer>`, `Map<Integer, PilotProfile>` | Stores pilot records and maintains fast dual-key indexing by callsign and ID. |
| [`LeaderboardService.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/service/LeaderboardService.java) | Line 93 & Line 145 | `Collections.sort(profiles)` | Automatically ranks pilots in descending order of high score using natural order `compareTo()`. |
| [`PilotProfile.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/model/PilotProfile.java) | Lines 57–67 | `public int compareTo(PilotProfile other)` | Implements `Comparable` interface defining natural sort order by `highScore` descending. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Line 89 & Lines 130–140 | `Set<Integer> pressedKeys = new HashSet<>()` | Tracks active keys with $O(1)$ `add()` and `remove()`, enabling smooth simultaneous diagonal flight. |
| [`GamePanel.java`](file:///Users/nishant/airstrikeenhance/PSOOP/src/ui/GamePanel.java) | Lines 180–190 | `Iterator<Particle> it = particles.iterator()` | Traverses visual particle spark list and safely removes expired sparks via `it.remove()`. |

---

## 6. How to Run & Verify

### 6.1 Automated Academic Test Suite (146 Tests)
Run from terminal or command prompt:
```bash
# On macOS / Linux:
./run_psoop_tests.sh

# On Windows:
run_psoop_tests.bat
```

### 6.2 Playable Java Swing Desktop Game
Launch the interactive desktop game:
```bash
# On macOS / Linux:
./run_psoop_swing.sh

# On Windows:
run_psoop_swing.bat
```
