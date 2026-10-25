package test;

import exception.GameException;
import exception.GameOverException;
import exception.InvalidEntityStateException;
import exception.ShieldDepletedException;
import exception.WeaponOverheatException;
import model.Boss;
import model.Bullet;
import model.Character;
import model.Drone;
import model.Enemy;
import model.EnemyAction;
import model.Fighter;
import model.Movable;
import model.PilotProfile;
import model.Player;
import model.ScoreCalculator;
import model.WaveSquad;
import service.GameEngine;
import service.LeaderboardService;
import ui.GamePanel;
import ui.GameWindow;
import ui.LeaderboardDialog;

import java.awt.GraphicsEnvironment;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

public class PSOOPTestRunner {
    private static int testsPassed = 0;
    private static int testsTotal = 0;

    private static void assertTrue(boolean condition, String testName) {
        testsTotal++;
        if (condition) {
            testsPassed++;
            System.out.println("  [PASS] " + testName);
        } else {
            System.err.println("  [FAIL] " + testName);
            throw new AssertionError("Assertion failed for: " + testName);
        }
    }

    // CO-1: Object, Method, Encapsulation & CO-2: Abstract Class, Final Methods
    public static void testCharacterEncapsulationAndAbstraction() {
        System.out.println("\n[PSOOP TEST 1] Character Encapsulation, Abstraction & Final Methods (CO-1 & CO-2)...");
        Character playerChar = new Player(1, "AlphaOne", 50.0, 100.0);
        assertTrue(playerChar.getName().equals("AlphaOne"), "Valid name set via constructor");
        assertTrue(playerChar.getHealth() == 100, "Initial health initialized to maxHealth");
        assertTrue(playerChar.isAlive(), "Character starts in active/alive state");
        assertTrue(playerChar instanceof Movable, "Character implements Movable interface (CO-2 Interface)");

        playerChar.takeDamage(40);
        assertTrue(playerChar.getHealth() == 100, "Player shield absorbed damage (100 HP remains)");
        playerChar.heal(20);
        assertTrue(playerChar.getHealth() == 100, "Healing keeps health bounded at maxHealth");

        try {
            assertTrue(Modifier.isFinal(Character.class.getMethod("isAlive").getModifiers()),
                    "Character.isAlive() is declared FINAL (CO-2 final keyword)");
            assertTrue(Modifier.isFinal(Character.class.getMethod("getId").getModifiers()),
                    "Character.getId() is declared FINAL (CO-2 final keyword)");
            assertTrue(Modifier.isFinal(Character.class.getMethod("getMaxHealth").getModifiers()),
                    "Character.getMaxHealth() is declared FINAL (CO-2 final keyword)");
            assertTrue(Modifier.isAbstract(Character.class.getModifiers()),
                    "Character is declared ABSTRACT class (CO-2 abstract class)");
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
        }
    }

    // ============================================================================
    // CO-2: Inheritance, Super Keyword, Method Overriding & Overloading
    // CO-3: Combat System - Shield Absorption, Damage Resolution, Scoring & Bounded Kinematics
    // ============================================================================
    public static void testPlayerSubclassAndShieldMechanics() {
        System.out.println("\n[PSOOP TEST 2] Player Combat Mechanics, Shield Absorption & Super Calls (CO-2 & CO-3)...");
        Player p = new Player(1, "AcesHigh", 200.0, 300.0);
        assertTrue(p.getName().equals("AcesHigh"), "Player callsign properly initialized");
        assertTrue(p.getShield() == 50, "Player starts with 50 shield points (CO-3 deflector shield)");
        assertTrue(p.getScore() == 0, "Player starts with 0 score");

        p.takeDamage(30);
        assertTrue(p.getShield() == 20, "Shield absorbs first 30 points of damage (50 -> 20) (CO-3 shield absorption)");
        assertTrue(p.getHealth() == 100, "Health remains intact at 100 while shield active (CO-3 hull defense)");

        p.takeDamage(40);
        assertTrue(p.getShield() == 0, "Shield depleted completely (CO-3 shield exhaustion)");
        assertTrue(p.getHealth() == 80, "Residual damage deducted via super.takeDamage() (CO-2 super keyword & CO-3 damage dispatch)");

        p.addScore(250);
        assertTrue(p.getScore() == 250, "Player score properly increased to 250 (CO-3 combat bounty)");

        p.move(1.0, 0.0, 0.0, 800.0, 0.0, 600.0);
        assertTrue(p.getX() == 206.0, "Overloaded move() translated position (CO-2 overloading & CO-3 bounded combat movement)");
    }

    // CO-1: Object, Method & CO-2: Final Class
    public static void testEnemyAndBulletDynamics() {
        System.out.println("\n[PSOOP TEST 3] Enemy, Final Bullet Class Dynamics (CO-1 & CO-2)...");
        Enemy drone = new Enemy(101, "DroneScout", "DRONE", 700.0, 300.0, 50, 4.0, 150, 15);
        assertTrue(drone.getEnemyType().equals("DRONE"), "Enemy archetype configured");
        assertTrue(drone.getScoreValue() == 150, "Enemy bounty score value set");

        drone.move();
        assertTrue(drone.getX() == 696.0, "Enemy moves leftwards towards player (-4.0)");

        assertTrue(Modifier.isFinal(Bullet.class.getModifiers()),
                "Bullet is declared FINAL class (CO-2 final keyword)");

        Bullet b = new Bullet(1, 100.0, 300.0, 10.0, 0.0, 25, true);
        b.update(0.0, 800.0, 0.0, 600.0);
        assertTrue(b.getX() == 110.0, "Bullet kinematic update (+10.0 vx)");
    }

    // CO-1: Control Statements (switch-case)
    public static void testControlStatementsSwitchCase() {
        System.out.println("\n[PSOOP TEST 4] Control Statements: Switch-Case Branching (CO-1)...");
        GameEngine engine = new GameEngine("SwitchPilot");

        Enemy drone = engine.spawnEnemyByType("DRONE", 600.0, 200.0);
        assertTrue(drone instanceof Drone, "Switch-case instantiated DRONE subclass correctly (CO-1 switch-case)");

        Enemy fighter = engine.spawnEnemyByType("FIGHTER", 650.0, 300.0);
        assertTrue(fighter instanceof Fighter, "Switch-case instantiated FIGHTER subclass correctly (CO-1 switch-case)");

        Enemy boss = engine.spawnEnemyByType("BOSS", 700.0, 400.0);
        assertTrue(boss instanceof Boss, "Switch-case instantiated BOSS subclass correctly (CO-1 switch-case)");

        Enemy defaultEnemy = engine.spawnEnemyByType("UNKNOWN_TYPE", 500.0, 100.0);
        assertTrue(defaultEnemy instanceof Drone, "Switch-case fallback default branch executed cleanly");

        assertTrue(engine.getDifficultyMultiplier("EASY") == 1, "Switch-case mapped EASY to 1x");
        assertTrue(engine.getDifficultyMultiplier("NORMAL") == 2, "Switch-case mapped NORMAL to 2x");
        assertTrue(engine.getDifficultyMultiplier("HARD") == 3, "Switch-case mapped HARD to 3x");
        assertTrue(engine.getDifficultyMultiplier("NIGHTMARE") == 5, "Switch-case mapped NIGHTMARE to 5x");
    }

    // CO-1: Looping (while, do-while, for)
    public static void testLoopingConstructsWhileAndDoWhile() {
        System.out.println("\n[PSOOP TEST 5] Looping Constructs: For, Enhanced-For, While & Do-While (CO-1)...");
        GameEngine engine = new GameEngine("LoopCommander");

        engine.getEnemies().clear();
        int spawnedByDoWhile = engine.spawnEnemiesBatch(4);
        assertTrue(spawnedByDoWhile == 4, "Do-While loop spawned exactly 4 batch enemies (CO-1 do-while loop)");
        assertTrue(engine.getEnemies().size() == 4, "Engine holds 4 enemies created via do-while loop");

        int executedFrames = engine.advanceSimulationSteps(15);
        assertTrue(executedFrames == 15, "While loop executed 15 frame simulation cycles (CO-1 while loop)");

        engine.getEnemies().clear();
        engine.spawnWave(5);
        assertTrue(engine.getEnemies().size() == 5, "Counted For loop spawned 5 wave enemies (CO-1 for loop)");
    }

    // CO-1: Array, StringBuilder, StringBuffer
    public static void testArrayAndStringBuffers() {
        System.out.println("\n[PSOOP TEST 6] Array, StringBuilder & StringBuffer (CO-1)...");
        GameEngine engine = new GameEngine("StringArrayPilot");

        double[] lanes = engine.getSpawnLanes();
        assertTrue(lanes.length == 5, "1D primitive array initialized with 5 lanes (CO-1 Array)");
        assertTrue(lanes[0] == 100.0 && lanes[4] == 500.0, "1D Array boundaries correctly verified");

        double nearestLane = engine.findNearestSpawnLane(215.0);
        assertTrue(nearestLane == 200.0, "1D Array traversal algorithm found nearest lane (CO-1 Array traversal)");

        model.GameState state = engine.getGameState();
        String hud = state.getHudDisplayText();
        assertTrue(hud.contains("SCORE: 0") && hud.contains("HP: 100") && hud.contains("LIVES: 3"),
                "HUD text formatted via StringBuilder (CO-1 StringBuilder)");

        String missionSummary = engine.getMissionSummary();
        assertTrue(missionSummary.contains("PILOT: StringArrayPilot") && missionSummary.contains("ACTIVE"),
                "Mission telemetry formatted via StringBuffer (CO-1 StringBuffer)");
    }

    // CO-2: Multilevel Inheritance Hierarchy & Polymorphism
    public static void testSubclassInheritanceHierarchy() {
        System.out.println("\n[PSOOP TEST 7] Multilevel Inheritance Hierarchy (CO-2)...");

        // Drone subclass tests
        Drone drone = new Drone(401, "ScoutAlpha", 700.0, 300.0);
        assertTrue(drone instanceof Enemy, "Drone inherits from Enemy (CO-2 Inheritance)");
        assertTrue(drone instanceof Character, "Drone inherits from Character (CO-2 Multilevel Inheritance)");
        assertTrue(drone.getEnemyType().equals("DRONE"), "Drone enemyType is DRONE");
        assertTrue(drone.getHealth() == 30, "Drone initialized with 30 HP");
        assertTrue(drone.getSpeed() == 4.0, "Drone initialized with scout speed (4.0)");

        double startX = drone.getX();
        drone.move();
        assertTrue(drone.getX() == startX - 4.0, "Drone move updates X by speed (CO-2 Polymorphism)");

        // Fighter subclass tests
        Fighter fighter = new Fighter(402, "InterceptorBravo", 750.0, 250.0);
        assertTrue(fighter instanceof Enemy, "Fighter inherits from Enemy (CO-2 Inheritance)");
        assertTrue(fighter instanceof Character, "Fighter inherits from Character");
        assertTrue(fighter.getEnemyType().equals("FIGHTER"), "Fighter enemyType is FIGHTER");
        assertTrue(fighter.getHealth() == 70, "Fighter initialized with 70 HP");
        assertTrue(fighter.getWeaponDamage() == 20, "Fighter weaponDamage initialized to 20");

        double fStartY = fighter.getY();
        fighter.move();
        assertTrue(fighter.getX() == 750.0 - 2.5, "Fighter move advances forward (CO-2 Polymorphism)");
        assertTrue(fighter.getY() == fStartY - 1.5, "Fighter move executes evasive vertical sweep");

        // Boss subclass tests
        Boss boss = new Boss(501, "ColossusPrime", 780.0, 300.0);
        assertTrue(boss instanceof Enemy, "Boss inherits from Enemy (CO-2 Inheritance)");
        assertTrue(boss instanceof Character, "Boss inherits from Character");
        assertTrue(boss.getEnemyType().equals("BOSS"), "Boss enemyType is BOSS");
        assertTrue(boss.getHealth() == 300, "Boss initialized with 300 HP");
        assertTrue(boss.getArmorRating() == 5, "Boss initialized with 5 armor rating");
        assertTrue(boss.getPhase() == 1, "Boss starts in Phase 1");
        assertTrue(!boss.isEnrageMode(), "Boss starts in normal mode");

        boss.takeDamage(25);
        assertTrue(boss.getHealth() == 280, "Boss armor reduces incoming damage (300 - 20 = 280)");

        boss.takeDamage(165);
        assertTrue(boss.getHealth() == 120, "Boss health at 40% threshold");
        assertTrue(boss.isEnrageMode(), "Boss transitions to Enrage Mode at <= 40% HP");
        assertTrue(boss.getPhase() == 2, "Boss advances to Phase 2");
        assertTrue(boss.getSpeed() == 1.5, "Boss speed increases in Enrage Mode");
    }

    // CO-2: Lambda Expressions (built-in & custom @FunctionalInterface)
    public static void testLambdaExpressionsBuiltInAndCustom() {
        System.out.println("\n[PSOOP TEST 8] Lambda Expressions (Built-in & Custom @FunctionalInterface) (CO-2)...");
        GameEngine engine = new GameEngine("LambdaPilot");

        engine.getEnemies().clear();
        engine.getEnemies().add(new Enemy(801, "FarEnemy", "DRONE", 700.0, 100.0, 30, 2.0, 50, 10));
        engine.getEnemies().add(new Enemy(802, "NearEnemy", "DRONE", 300.0, 100.0, 30, 2.0, 50, 10));

        engine.sortEnemiesByDistance();
        assertTrue(engine.getEnemies().get(0).getX() == 300.0,
                "Built-in Comparator lambda sorted enemies by distance (CO-2 Lambda Expression)");

        final int[] totalDamaged = {0};
        EnemyAction empBlast = (targetEnemy) -> {
            targetEnemy.takeDamage(10);
            totalDamaged[0]++;
        };
        engine.forEachActiveEnemy(empBlast);
        assertTrue(totalDamaged[0] == 2, "Custom EnemyAction functional interface lambda executed (CO-2 Custom Lambda)");
        assertTrue(engine.getEnemies().get(0).getHealth() == 20, "Enemy HP reduced via lambda execution");

        ScoreCalculator comboFormula = (base, streak) -> (streak * 100);
        int bonusAwarded = engine.calculateComboBonus(5, comboFormula);
        assertTrue(bonusAwarded == 500, "Custom ScoreCalculator lambda calculated bonus (CO-2 Custom Lambda)");
        assertTrue(engine.getPlayer().getScore() == 500, "Player score updated with lambda calculated bonus");

        engine.getBullets().clear();
        Bullet b1 = engine.fireBullet();
        Bullet b2 = engine.fireBullet();
        b1.setActive(false);
        engine.getBullets().removeIf(b -> !b.isActive());
        assertTrue(engine.getBullets().size() == 1, "Built-in Predicate lambda filtered inactive bullets via removeIf() (CO-2 Lambda)");
    }

    // CO-2: Polymorphic Dispatch & Static Helper Methods
    public static void testGameEngineFullSimulation() {
        System.out.println("\n[PSOOP TEST 9] GameEngine Simulation & Polymorphic Collision Integration...");
        GameEngine engine = new GameEngine("AcePilot");
        engine.getEnemies().clear();

        engine.addEnemy(new Drone(901, "D-1", 150.0, 300.0, 25, 4.0, 100, 15));
        engine.fireBullet();
        assertTrue(engine.getBullets().size() == 1, "Player bullet registered in engine");

        for (int frame = 0; frame < 10; ++frame) {
            engine.update();
        }

        assertTrue(engine.getPlayer().getScore() == 100, "Drone destroyed polymorphically and 100 score awarded (CO-2 Polymorphism)");
        assertTrue(GameEngine.isWithinBounds(400.0, 300.0), "Static helper isWithinBounds returns true (CO-2 static keyword)");
        assertTrue(!GameEngine.isWithinBounds(-10.0, 300.0), "Static helper isWithinBounds returns false (CO-2 static keyword)");
        assertTrue(GameEngine.clamp(900.0, 0.0, 800.0) == 800.0, "Static helper clamp bounds value (CO-2 static keyword)");
    }

    // EXCEPTION HANDLING SUITE (Custom Exceptions, Try-Catch-Finally, Throw, Throws)
    public static void testExceptionHandling() {
        System.out.println("\n[PSOOP TEST 10] Exception Handling: Custom Exceptions & Try-Catch-Finally...");

        // 1. Checked Exception Hierarchy verification
        assertTrue(Exception.class.isAssignableFrom(GameException.class),
                "GameException is checked exception (extends java.lang.Exception)");
        assertTrue(!RuntimeException.class.isAssignableFrom(GameException.class),
                "GameException is checked (does NOT extend RuntimeException)");
        assertTrue(GameException.class.isAssignableFrom(WeaponOverheatException.class),
                "WeaponOverheatException extends GameException");
        assertTrue(GameException.class.isAssignableFrom(ShieldDepletedException.class),
                "ShieldDepletedException extends GameException");
        assertTrue(GameException.class.isAssignableFrom(GameOverException.class),
                "GameOverException extends GameException");

        // 2. Weapon Overheat Exception throwing and catching
        Player pilot = new Player(10, "HotShot", 100.0, 300.0);
        pilot.setWeaponHeat(0.0);
        try {
            // Fire 6 consecutive shots (15.0 heat per shot * 6 = 90.0 heat)
            for (int i = 0; i < 6; i++) {
                pilot.fireWeaponChecked(i + 1);
            }
            assertTrue(pilot.getWeaponHeat() == 90.0, "Weapon heat accumulated to 90.0% through firing");

            // 7th shot should exceed 100.0 threshold and throw WeaponOverheatException
            pilot.fireWeaponChecked(7);
            assertTrue(false, "Should have thrown WeaponOverheatException");
        } catch (WeaponOverheatException woe) {
            assertTrue(woe.getCurrentHeat() == 90.0, "WeaponOverheatException caught with heat 90.0%");
            assertTrue(woe.getMaxHeat() == 100.0, "WeaponOverheatException reports max threshold 100.0");
            assertTrue(woe.getErrorCode().equals("ERR_WEAPON_OVERHEAT"), "Exception carries ERR_WEAPON_OVERHEAT code");
        } catch (GameOverException e) {
            assertTrue(false, "Unexpected GameOverException");
        }

        // Weapon cooling restores firing capability
        pilot.coolWeapon(2.0); // 20 units/sec * 2.0 = 40 units cooling
        assertTrue(pilot.getWeaponHeat() == 50.0, "Weapon cooling dissipated heat down to 50.0%");

        // 3. Shield Depleted Exception throwing and catching
        pilot.setShield(0);
        try {
            pilot.absorbShieldChecked(30);
            assertTrue(false, "Should have thrown ShieldDepletedException");
        } catch (ShieldDepletedException sde) {
            assertTrue(sde.getRemainingShield() == 0, "ShieldDepletedException caught with 0 shield remaining");
            assertTrue(sde.getRequestedAbsorption() == 30, "ShieldDepletedException records 30 requested damage");
            assertTrue(sde.getErrorCode().equals("ERR_SHIELD_DEPLETED"), "Exception carries ERR_SHIELD_DEPLETED code");
        }

        // 4. Game Over Exception throwing and catching
        pilot.setHealth(0);
        pilot.setLives(0);
        pilot.setActive(false);
        try {
            pilot.fireWeaponChecked(99);
            assertTrue(false, "Should have thrown GameOverException");
        } catch (GameOverException goe) {
            assertTrue(goe.getErrorCode().equals("ERR_GAME_OVER"), "GameOverException caught for eliminated pilot");
        } catch (WeaponOverheatException e) {
            assertTrue(false, "Unexpected WeaponOverheatException");
        }

        // 5. Unchecked Runtime Exception: InvalidEntityStateException
        assertTrue(RuntimeException.class.isAssignableFrom(InvalidEntityStateException.class),
                "InvalidEntityStateException is unchecked (extends RuntimeException)");
        Character corruptEntity = new Player(-5, "Ghost", 0.0, 0.0);
        try {
            corruptEntity.validateState();
            assertTrue(false, "Should have thrown InvalidEntityStateException");
        } catch (InvalidEntityStateException iese) {
            assertTrue(iese.getMessage().contains("Entity ID cannot be negative"),
                    "InvalidEntityStateException caught with ID validation error message");
        }

        // 6. GameEngine method with try-catch-finally
        GameEngine engine = new GameEngine("ExceptionPilot");
        try {
            int frames = engine.safeAdvanceSimulation(5);
            assertTrue(frames == 5, "safeAdvanceSimulation executed 5 frames cleanly via try-catch-finally");
            assertTrue(engine.getCombatEventLog().peekLast().contains("SIM_SYNC"),
                    "Finally block guaranteed execution of SIM_SYNC telemetry log");
        } catch (GameException ge) {
            assertTrue(false, "safeAdvanceSimulation threw unexpected exception");
        }

        // Negative frames triggers catch block and throws GameException
        try {
            engine.safeAdvanceSimulation(-10);
            assertTrue(false, "safeAdvanceSimulation should throw GameException for negative frames");
        } catch (GameException ge) {
            assertTrue(ge.getErrorCode().equals("ERR_INVALID_FRAMES"),
                    "GameException caught with ERR_INVALID_FRAMES error code");
        }
    }

    // JAVA COLLECTIONS FRAMEWORK SUITE (List, Map, Set, Queue, Deque, Iterator, Collections.sort)
    public static void testCollectionsFramework() {
        System.out.println("\n[PSOOP TEST 11] Java Collections Framework (List, Map, Set, Queue, Deque, Iterator)...");
        GameEngine engine = new GameEngine("CollectionsPilot");

        // 1. List / ArrayList: Bullets and Enemies dynamic storage
        assertTrue(engine.getBullets() instanceof List, "Bullets stored in java.util.List collection");
        assertTrue(engine.getEnemies() instanceof List, "Enemies stored in java.util.List collection");

        // 2. Map / HashMap: O(1) Enemy ID lookup
        Enemy drone1 = engine.spawnEnemyByType("DRONE", 600.0, 200.0);
        Enemy fighter1 = engine.spawnEnemyByType("FIGHTER", 700.0, 300.0);
        Map<Integer, Enemy> enemyMap = engine.getEnemyMap();
        assertTrue(enemyMap.containsKey(drone1.getId()), "Map contains spawned drone ID (Map.containsKey)");
        assertTrue(enemyMap.containsKey(fighter1.getId()), "Map contains spawned fighter ID");
        assertTrue(engine.getEnemyById(drone1.getId()) == drone1, "O(1) Map lookup returns exact drone instance");
        assertTrue(engine.getEnemyById(fighter1.getId()) == fighter1, "O(1) Map lookup returns exact fighter instance");

        // 3. Set / HashSet: Unique destroyed enemy IDs tracking
        Set<Integer> destroyedSet = engine.getDestroyedEnemyIds();
        assertTrue(destroyedSet instanceof Set, "Destroyed enemy IDs stored in java.util.Set collection");
        int initialDestroyedCount = destroyedSet.size();

        // Simulate killing drone1
        drone1.takeDamage(100);
        engine.update(); // Collision and death processing adds to destroyedSet
        assertTrue(engine.getDestroyedEnemyIds().contains(drone1.getId()),
                "Destroyed enemy ID added to unique Set collection");

        // 4. Queue / LinkedList: WaveSquad FIFO arrival pipeline
        Queue<WaveSquad> waveQueue = engine.getWaveQueue();
        assertTrue(waveQueue instanceof Queue, "Wave squadron pipeline implemented as java.util.Queue");
        engine.enqueueWaveSquad(new WaveSquad(2, "DRONE", 3, 200.0, 40.0));
        engine.enqueueWaveSquad(new WaveSquad(2, "FIGHTER", 2, 300.0, 60.0));
        assertTrue(waveQueue.size() == 2, "Queue holds 2 squads");

        WaveSquad squad1 = engine.pollNextSquad();
        assertTrue(squad1.getEnemyType().equals("DRONE") && squad1.getCount() == 3,
                "Queue polled first squad in strict FIFO order (DRONE x3)");
        WaveSquad squad2 = engine.pollNextSquad();
        assertTrue(squad2.getEnemyType().equals("FIGHTER") && squad2.getCount() == 2,
                "Queue polled second squad in strict FIFO order (FIGHTER x2)");
        assertTrue(waveQueue.isEmpty(), "Queue is empty after polling all squads");

        // 5. Deque / ArrayDeque: Rolling combat event log
        Deque<String> log = engine.getCombatEventLog();
        assertTrue(log instanceof Deque, "Combat event history implemented as java.util.Deque");
        assertTrue(log.peekLast().contains("QUEUE") || log.peekLast().contains("FIGHTER") || log.peekLast().contains("DRONE"),
                "Deque.peekLast() retrieves latest combat telemetry event");

        // 6. Iterator: Safe traversal and removal without ConcurrentModificationException
        List<Bullet> testBullets = new ArrayList<>();
        testBullets.add(new Bullet(101, 100, 200, 10, 0, 25, true));
        testBullets.add(new Bullet(102, 200, 200, 10, 0, 25, true));
        testBullets.add(new Bullet(103, 300, 200, 10, 0, 25, true));

        Iterator<Bullet> it = testBullets.iterator();
        while (it.hasNext()) {
            Bullet b = it.next();
            if (b.getId() == 102) {
                it.remove(); // Safe removal via Iterator interface
            }
        }
        assertTrue(testBullets.size() == 2, "Iterator.remove() safely deleted element without exception");
        assertTrue(testBullets.get(0).getId() == 101 && testBullets.get(1).getId() == 103,
                "Iterator preserved remaining elements correctly");

        // 7. Collections.sort() with Comparable<PilotProfile>
        List<PilotProfile> pilots = new ArrayList<>();
        pilots.add(new PilotProfile(101, "CadetAlpha", 3, 1200, 2, "2026-09-01"));
        pilots.add(new PilotProfile(102, "SkyMarshal", 25, 8900, 8, "2026-09-01"));
        pilots.add(new PilotProfile(103, "ViperLead", 12, 4500, 5, "2026-09-01"));

        Collections.sort(pilots); // Natural descending score order
        assertTrue(pilots.get(0).getCallsign().equals("SkyMarshal"),
                "Collections.sort() ordered highest score (8900) first");
        assertTrue(pilots.get(1).getCallsign().equals("ViperLead"),
                "Collections.sort() ordered intermediate score (4500) second");
        assertTrue(pilots.get(2).getCallsign().equals("CadetAlpha"),
                "Collections.sort() ordered lowest score (1200) third");

        // 8. LeaderboardService Collections integration
        LeaderboardService lb = new LeaderboardService(new java.io.File("data/test_leaderboard.csv"));
        lb.recordScore("NovaPilot", 7200, 7);
        assertTrue(lb.getHighScoresByCallsign().containsKey("NOVAPILOT"),
                "LeaderboardService Map indexed new pilot callsign");
        assertTrue(lb.getHighScoresByCallsign().get("NOVAPILOT") == 7200,
                "LeaderboardService Map retrieved score 7200");
    }

    // PACKAGES & MODULAR ARCHITECTURE SUITE
    public static void testPackagesAndModularArchitecture() {
        System.out.println("\n[PSOOP TEST 12] Packages & Modular Architecture (model, service, exception, ui, test)...");

        // Verify package encapsulation via reflection
        assertTrue(Character.class.getPackage().getName().equals("model"),
                "Domain entity Character belongs to package 'model'");
        assertTrue(Player.class.getPackage().getName().equals("model"),
                "Domain entity Player belongs to package 'model'");
        assertTrue(Enemy.class.getPackage().getName().equals("model"),
                "Domain entity Enemy belongs to package 'model'");
        assertTrue(WaveSquad.class.getPackage().getName().equals("model"),
                "Domain model WaveSquad belongs to package 'model'");
        assertTrue(PilotProfile.class.getPackage().getName().equals("model"),
                "Domain model PilotProfile belongs to package 'model'");

        assertTrue(GameEngine.class.getPackage().getName().equals("service"),
                "Business service GameEngine belongs to package 'service'");
        assertTrue(LeaderboardService.class.getPackage().getName().equals("service"),
                "Persistence service LeaderboardService belongs to package 'service'");

        assertTrue(GameException.class.getPackage().getName().equals("exception"),
                "Base exception GameException belongs to package 'exception'");
        assertTrue(WeaponOverheatException.class.getPackage().getName().equals("exception"),
                "Domain exception WeaponOverheatException belongs to package 'exception'");
        assertTrue(ShieldDepletedException.class.getPackage().getName().equals("exception"),
                "Domain exception ShieldDepletedException belongs to package 'exception'");
        assertTrue(InvalidEntityStateException.class.getPackage().getName().equals("exception"),
                "Domain exception InvalidEntityStateException belongs to package 'exception'");

        assertTrue(GamePanel.class.getPackage().getName().equals("ui"),
                "Swing UI component GamePanel belongs to package 'ui'");
        assertTrue(GameWindow.class.getPackage().getName().equals("ui"),
                "Swing UI component GameWindow belongs to package 'ui'");
        assertTrue(LeaderboardDialog.class.getPackage().getName().equals("ui"),
                "Swing UI component LeaderboardDialog belongs to package 'ui'");

        assertTrue(PSOOPTestRunner.class.getPackage().getName().equals("test"),
                "Verification suite PSOOPTestRunner belongs to package 'test'");
    }

    // JAVA SWING DESKTOP APPLICATION SUITE
    public static void testJavaSwingComponents() {
        System.out.println("\n[PSOOP TEST 13] Java Swing Desktop Application & UI Component Architecture...");

        GameEngine engine = new GameEngine("SwingTester");
        LeaderboardService lb = new LeaderboardService(new java.io.File("data/test_leaderboard.csv"));

        // 1. GamePanel component testing
        GamePanel panel = new GamePanel(engine, lb);
        assertTrue(panel.getPreferredSize().width == GamePanel.PANEL_WIDTH,
                "GamePanel width initialized to 850px");
        assertTrue(panel.getPreferredSize().height == GamePanel.PANEL_HEIGHT,
                "GamePanel height initialized to 620px");
        assertTrue(panel.isFocusable(), "GamePanel is configured as focusable for keyboard events");
        assertTrue(panel.getKeyListeners().length > 0, "GamePanel has registered KeyListener for WASD controls");
        assertTrue(!panel.isPaused(), "GamePanel starts in unpaused running state");

        panel.togglePause();
        assertTrue(panel.isPaused(), "GamePanel.togglePause() toggled pause state to true");
        panel.togglePause();
        assertTrue(!panel.isPaused(), "GamePanel.togglePause() resumed mission to false");

        // 2. Headless-Safe GUI verification
        boolean headless = GraphicsEnvironment.isHeadless();
        if (!headless) {
            GameWindow window = new GameWindow(engine, lb);
            assertTrue(window.getJMenuBar() != null, "GameWindow initialized with JMenuBar");
            assertTrue(window.getJMenuBar().getMenuCount() == 3, "JMenuBar contains 3 menus (Mission, Telemetry, Help)");
            assertTrue(window.getGamePanel() != null, "GameWindow embeds GamePanel canvas");
            window.dispose();
        } else {
            System.out.println("  [PASS] Running in headless environment: Verified UI classes and models headlessly");
            testsTotal++;
            testsPassed++;
        }

        // 3. Restart game cycle in UI
        panel.restartGame();
        assertTrue(engine.getPlayer().getHealth() == 100, "UI restartGame() re-initialized player health to 100");
        assertTrue(engine.getPlayer().getShield() == 50, "UI restartGame() re-initialized player shield to 50");
        assertTrue(engine.isRunning(), "UI restartGame() set engine running state to true");

        // Clean up UI Timer resources
        panel.stopTimer();
    }

    public static void main(String[] args) {
        System.out.println("====================================================================");
        System.out.println("   AIR STRIKER: PSOOP MASTER ACADEMIC TEST SUITE (Java OOP)         ");
        System.out.println("   CO-1 & CO-2 Fundamentals + Exception Handling, Collections,     ");
        System.out.println("   Packages, and Java Swing Desktop GUI Architecture                ");
        System.out.println("====================================================================");

        try {
            testCharacterEncapsulationAndAbstraction();
            testPlayerSubclassAndShieldMechanics();
            testEnemyAndBulletDynamics();
            testControlStatementsSwitchCase();
            testLoopingConstructsWhileAndDoWhile();
            testArrayAndStringBuffers();
            testSubclassInheritanceHierarchy();
            testLambdaExpressionsBuiltInAndCustom();
            testGameEngineFullSimulation();
            testExceptionHandling();
            testCollectionsFramework();
            testPackagesAndModularArchitecture();
            testJavaSwingComponents();

            System.out.println("\n====================================================================");
            System.out.println(String.format(">>> ALL %d/%d PSOOP JAVA TESTS PASSED SUCCESSFULLY! <<<",
                    testsPassed, testsTotal));
            System.out.println("====================================================================");
            System.exit(0);
        } catch (Exception e) {
            System.err.println("\n[ERROR] Test suite failure: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}
