package test;

import model.Boss;
import model.Bullet;
import model.Character;
import model.Drone;
import model.Enemy;
import model.Fighter;
import model.Movable;
import model.Player;
import service.GameEngine;

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

    public static void testCharacterEncapsulationAndAbstraction() {
        System.out.println("\n[PSOOP TEST 1] Character Encapsulation & Polymorphism...");
        Character playerChar = new Player(1, "AlphaOne", 50.0, 100.0);
        assertTrue(playerChar.getName().equals("AlphaOne"), "Valid name set via constructor");
        assertTrue(playerChar.getHealth() == 100, "Initial health initialized to maxHealth");
        assertTrue(playerChar.isAlive(), "Character starts in active/alive state");
        assertTrue(playerChar instanceof Movable, "Character implements Movable interface");

        playerChar.takeDamage(40);
        assertTrue(playerChar.getHealth() == 100, "Player shield absorbed damage (100 HP remains)");
        playerChar.heal(20);
        assertTrue(playerChar.getHealth() == 100, "Healing keeps health bounded at maxHealth");
    }

    public static void testPlayerSubclassAndShieldMechanics() {
        System.out.println("\n[PSOOP TEST 2] Player Subclass & Shield Absorption...");
        Player p = new Player(1, "AcesHigh", 200.0, 300.0);
        assertTrue(p.getName().equals("AcesHigh"), "Player callsign properly initialized");
        assertTrue(p.getShield() == 50, "Player starts with 50 shield points");
        assertTrue(p.getScore() == 0, "Player starts with 0 score");

        p.takeDamage(30);
        assertTrue(p.getShield() == 20, "Shield absorbs first 30 points of damage (50 -> 20)");
        assertTrue(p.getHealth() == 100, "Health remains intact at 100 while shield active");

        p.takeDamage(40);
        assertTrue(p.getShield() == 0, "Shield depleted completely");
        assertTrue(p.getHealth() == 80, "Residual damage deducted from player health");

        p.addScore(250);
        assertTrue(p.getScore() == 250, "Player score properly increased to 250");
    }

    public static void testEnemyAndBulletDynamics() {
        System.out.println("\n[PSOOP TEST 3] Enemy & Bullet Dynamics...");
        Enemy drone = new Enemy(101, "DroneScout", "DRONE", 700.0, 300.0, 50, 4.0, 150, 15);
        assertTrue(drone.getEnemyType().equals("DRONE"), "Enemy archetype configured");
        assertTrue(drone.getScoreValue() == 150, "Enemy bounty score value set");

        drone.move();
        assertTrue(drone.getX() == 696.0, "Enemy moves leftwards towards player (-4.0)");

        Bullet b = new Bullet(1, 100.0, 300.0, 10.0, 0.0, 25, true);
        b.update(0.0, 800.0, 0.0, 600.0);
        assertTrue(b.getX() == 110.0, "Bullet kinematic update (+10.0 vx)");
    }

    public static void testGameEngineFullSimulation() {
        System.out.println("\n[PSOOP TEST 4] GameEngine Object Interaction & Simulation...");
        GameEngine engine = new GameEngine("SkyCommander");
        assertTrue(engine.getPlayer().getName().equals("SkyCommander"), "Engine initialized player name");

        engine.getEnemies().clear();
        Enemy target = new Enemy(201, "TargetDrone", "DRONE", 150.0, 300.0, 20, 0.0, 100, 10);
        engine.getEnemies().add(target);

        engine.fireBullet();
        assertTrue(engine.getBullets().size() == 1, "Player bullet registered in engine");

        for (int frame = 0; frame < 10; ++frame) {
            engine.update();
        }

        assertTrue(engine.getPlayer().getScore() == 100, "Enemy destroyed and 100 score awarded to player");

        model.GameState state = engine.getGameState();
        assertTrue(state.getScore() == 100, "GameState synced with player score");
        assertTrue(state.getHealth() == 100, "GameState reflects player health");
    }

    public static void testStringHandlersAndUtilities() {
        System.out.println("\n[PSOOP TEST 5] String Handlers, Array & Utilities...");
        GameEngine engine = new GameEngine("ViperLead");

        double[] lanes = engine.getSpawnLanes();
        assertTrue(lanes.length == 5, "Spawn lanes array initialized with 5 positions");
        assertTrue(lanes[0] == 100.0 && lanes[4] == 500.0, "Spawn lane boundaries correctly indexed");

        model.GameState state = engine.getGameState();
        String hud = state.getHudDisplayText();
        assertTrue(hud.contains("SCORE: 0") && hud.contains("HP: 100"), "HUD display built using StringBuilder");

        String summary = engine.getMissionSummary();
        assertTrue(summary.contains("PILOT: ViperLead") && summary.contains("ACTIVE"), "Mission summary built using StringBuffer");

        assertTrue(GameEngine.isWithinBounds(400.0, 300.0), "Static boundary check within bounds");
        assertTrue(!GameEngine.isWithinBounds(-10.0, 300.0), "Static boundary check out of bounds");

        engine.getEnemies().clear();
        engine.getEnemies().add(new Enemy(301, "DroneFar", "DRONE", 500.0, 200.0, 20, 2.0, 50, 10));
        engine.getEnemies().add(new Enemy(302, "DroneNear", "DRONE", 200.0, 200.0, 20, 2.0, 50, 10));
        engine.sortEnemiesByDistance();
        assertTrue(engine.getEnemies().get(0).getX() == 200.0, "Lambda comparator sorted enemies by distance");
    }

    public static void testSubclassInheritanceHierarchy() {
        System.out.println("\n[PSOOP TEST 6] Drone, Fighter, Boss Inheritance Hierarchy...");

        // 1. Drone subclass tests
        Drone drone = new Drone(401, "ScoutAlpha", 700.0, 300.0);
        assertTrue(drone instanceof Enemy, "Drone inherits from Enemy");
        assertTrue(drone instanceof Character, "Drone inherits from Character");
        assertTrue(drone.getEnemyType().equals("DRONE"), "Drone enemyType is DRONE");
        assertTrue(drone.getHealth() == 30, "Drone initialized with 30 HP");
        assertTrue(drone.getSpeed() == 4.0, "Drone initialized with high scout speed (4.0)");

        double startX = drone.getX();
        drone.move();
        assertTrue(drone.getX() == startX - 4.0, "Drone move updates X by speed");
        assertTrue(drone.getY() != 300.0 || drone.getOscillationAmplitude() > 0, "Drone oscillates along Y-axis");

        // 2. Fighter subclass tests
        Fighter fighter = new Fighter(402, "InterceptorBravo", 750.0, 250.0);
        assertTrue(fighter instanceof Enemy, "Fighter inherits from Enemy");
        assertTrue(fighter instanceof Character, "Fighter inherits from Character");
        assertTrue(fighter.getEnemyType().equals("FIGHTER"), "Fighter enemyType is FIGHTER");
        assertTrue(fighter.getHealth() == 70, "Fighter initialized with 70 HP");
        assertTrue(fighter.getWeaponDamage() == 20, "Fighter weaponDamage initialized to 20");

        double fStartY = fighter.getY();
        fighter.move();
        assertTrue(fighter.getX() == 750.0 - 2.5, "Fighter move advances forward");
        assertTrue(fighter.getY() == fStartY - 1.5, "Fighter move executes evasive vertical sweep");

        // 3. Boss subclass tests
        Boss boss = new Boss(501, "ColossusPrime", 780.0, 300.0);
        assertTrue(boss instanceof Enemy, "Boss inherits from Enemy");
        assertTrue(boss instanceof Character, "Boss inherits from Character");
        assertTrue(boss.getEnemyType().equals("BOSS"), "Boss enemyType is BOSS");
        assertTrue(boss.getHealth() == 300, "Boss initialized with 300 HP");
        assertTrue(boss.getArmorRating() == 5, "Boss initialized with 5 armor rating");
        assertTrue(boss.getPhase() == 1, "Boss starts in Phase 1");
        assertTrue(!boss.isEnrageMode(), "Boss starts in normal mode (not enraged)");

        // Armor mitigation test: 25 damage with 5 armor = 20 effective damage dealt
        boss.takeDamage(25);
        assertTrue(boss.getHealth() == 280, "Boss armor reduces incoming damage (300 - 20 = 280)");

        // Enrage transition test: deal damage down to <= 40% health
        boss.takeDamage(165); // 165 - 5 = 160 damage -> 280 - 160 = 120 (40% of 300)
        assertTrue(boss.getHealth() == 120, "Boss health at 40% threshold");
        assertTrue(boss.isEnrageMode(), "Boss transitions to Enrage Mode at <= 40% HP");
        assertTrue(boss.getPhase() == 2, "Boss advances to Phase 2");
        assertTrue(boss.getSpeed() == 1.5, "Boss speed increases in Enrage Mode");
    }

    public static void testPolymorphicEngineIntegration() {
        System.out.println("\n[PSOOP TEST 7] Polymorphic Enemy Integration in GameEngine...");
        GameEngine engine = new GameEngine("AcePilot");
        engine.getEnemies().clear();

        // Polymorphic list holding Drone, Fighter, Boss references via base Enemy type
        engine.addEnemy(new Drone(601, "D-1", 700.0, 100.0));
        engine.addEnemy(new Fighter(602, "F-1", 700.0, 200.0));
        engine.addEnemy(new Boss(603, "B-1", 700.0, 300.0));

        assertTrue(engine.getEnemies().size() == 3, "Engine holds 3 polymorphic enemies");

        // Polymorphic movement execution
        for (Enemy e : engine.getEnemies()) {
            double prevX = e.getX();
            e.move();
            assertTrue(e.getX() < prevX, "Polymorphic move invoked correctly for " + e.getClass().getSimpleName());
        }

        // Test boss spawning helper
        engine.spawnBoss();
        assertTrue(engine.getEnemies().size() == 4, "Engine spawned additional Boss");
        Enemy lastEnemy = engine.getEnemies().get(3);
        assertTrue(lastEnemy instanceof Boss, "Spawned enemy is instance of Boss");
    }

    public static void main(String[] args) {
        System.out.println("=====================================================");
        System.out.println("   AIR STRIKER: PSOOP TEST SUITE (Java OOP)          ");
        System.out.println("=====================================================");

        try {
            testCharacterEncapsulationAndAbstraction();
            testPlayerSubclassAndShieldMechanics();
            testEnemyAndBulletDynamics();
            testGameEngineFullSimulation();
            testStringHandlersAndUtilities();
            testSubclassInheritanceHierarchy();
            testPolymorphicEngineIntegration();

            System.out.println("\n=====================================================");
            System.out.println(String.format(">>> ALL %d/%d PSOOP JAVA TESTS PASSED SUCCESSFULLY! <<<",
                    testsPassed, testsTotal));
            System.out.println("=====================================================");
        } catch (Exception e) {
            System.err.println("\n[ERROR] Test suite failure: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}

