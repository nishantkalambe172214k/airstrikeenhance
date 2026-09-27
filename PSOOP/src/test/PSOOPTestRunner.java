package test;

import model.Bullet;
import model.Character;
import model.Enemy;
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

    public static void main(String[] args) {
        System.out.println("=====================================================");
        System.out.println("   AIR STRIKER: PSOOP TEST SUITE (Java OOP)          ");
        System.out.println("=====================================================");

        try {
            testCharacterEncapsulationAndAbstraction();
            testPlayerSubclassAndShieldMechanics();
            testEnemyAndBulletDynamics();
            testGameEngineFullSimulation();

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
