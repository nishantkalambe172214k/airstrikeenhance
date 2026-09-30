package test;

import model.Boss;
import model.Bullet;
import model.Character;
import model.Drone;
import model.Enemy;
import model.EnemyAction;
import model.Fighter;
import model.Movable;
import model.Player;
import model.ScoreCalculator;
import service.GameEngine;

import java.lang.reflect.Modifier;

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

    // CO-2: Inheritance, Super Keyword, Method Overriding & Overloading
    public static void testPlayerSubclassAndShieldMechanics() {
        System.out.println("\n[PSOOP TEST 2] Player Subclass, Shield Absorption & Super Calls (CO-2)...");
        Player p = new Player(1, "AcesHigh", 200.0, 300.0);
        assertTrue(p.getName().equals("AcesHigh"), "Player callsign properly initialized");
        assertTrue(p.getShield() == 50, "Player starts with 50 shield points");
        assertTrue(p.getScore() == 0, "Player starts with 0 score");

        p.takeDamage(30);
        assertTrue(p.getShield() == 20, "Shield absorbs first 30 points of damage (50 -> 20)");
        assertTrue(p.getHealth() == 100, "Health remains intact at 100 while shield active");

        p.takeDamage(40);
        assertTrue(p.getShield() == 0, "Shield depleted completely");
        assertTrue(p.getHealth() == 80, "Residual damage deducted via super.takeDamage() (CO-2 super keyword)");

        p.addScore(250);
        assertTrue(p.getScore() == 250, "Player score properly increased to 250");

        p.move(1.0, 0.0, 0.0, 800.0, 0.0, 600.0);
        assertTrue(p.getX() == 206.0, "Overloaded move() translated position (CO-2 compile-time polymorphism)");
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

    public static void main(String[] args) {
        System.out.println("====================================================================");
        System.out.println("   AIR STRIKER: PSOOP CO-1 & CO-2 COMPLETE TEST SUITE (Java OOP)    ");
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

            System.out.println("\n====================================================================");
            System.out.println(String.format(">>> ALL %d/%d PSOOP JAVA TESTS PASSED SUCCESSFULLY (CO-1 & CO-2)! <<<",
                    testsPassed, testsTotal));
            System.out.println("====================================================================");
        } catch (Exception e) {
            System.err.println("\n[ERROR] Test suite failure: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}
