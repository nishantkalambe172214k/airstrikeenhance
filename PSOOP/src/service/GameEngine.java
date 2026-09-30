package service;

import model.Boss;
import model.Bullet;
import model.Drone;
import model.Enemy;
import model.EnemyAction;
import model.Fighter;
import model.GameState;
import model.Player;
import model.ScoreCalculator;

import java.util.ArrayList;
import java.util.List;

public class GameEngine {
    // CO-2: Static Final constants
    public static final double SCREEN_MIN_X = 0.0;
    public static final double SCREEN_MAX_X = 800.0;
    public static final double SCREEN_MIN_Y = 0.0;
    public static final double SCREEN_MAX_Y = 600.0;

    // CO-1: 1D Array for spatial lanes
    private static final double[] SPAWN_LANES = { 100.0, 200.0, 300.0, 400.0, 500.0 };

    private Player player;
    private List<Enemy> enemies;
    private List<Bullet> bullets;
    private int nextBulletId;
    private int nextEnemyId;
    private int currentWave;
    private boolean running;

    public GameEngine() {
        this("CadetPilot");
    }

    public GameEngine(final String pilotName) {
        this.player = new Player(1, pilotName, 100.0, 300.0);
        this.enemies = new ArrayList<>();
        this.bullets = new ArrayList<>();
        this.nextBulletId = 1;
        this.nextEnemyId = 100;
        this.currentWave = 1;
        this.running = true;
    }

    // CO-2: Static helper methods
    public static boolean isWithinBounds(final double x, final double y) {
        return x >= SCREEN_MIN_X && x <= SCREEN_MAX_X && y >= SCREEN_MIN_Y && y <= SCREEN_MAX_Y;
    }

    public static double clamp(final double value, final double min, final double max) {
        return Math.max(min, Math.min(max, value));
    }

    public double[] getSpawnLanes() {
        return SPAWN_LANES;
    }

    // CO-1: 1D Array traversal method
    public double findNearestSpawnLane(final double targetY) {
        double closest = SPAWN_LANES[0];
        double minDiff = Math.abs(targetY - closest);
        for (int i = 1; i < SPAWN_LANES.length; i++) {
            double diff = Math.abs(targetY - SPAWN_LANES[i]);
            if (diff < minDiff) {
                minDiff = diff;
                closest = SPAWN_LANES[i];
            }
        }
        return closest;
    }

    // CO-1: Control Statement - Switch-Case factory for enemy types
    public Enemy spawnEnemyByType(final String enemyType, final double x, final double y) {
        final int id = nextEnemyId++;
        final String typeKey = (enemyType != null) ? enemyType.trim().toUpperCase() : "DRONE";
        Enemy enemy;

        switch (typeKey) {
            case "FIGHTER":
                enemy = new Fighter(id, "Interceptor-" + id, x, y);
                break;
            case "BOSS":
                enemy = new Boss(id, "Dreadnought-" + id, x, y);
                break;
            case "DRONE":
            default:
                enemy = new Drone(id, "Scout-" + id, x, y);
                break;
        }

        enemies.add(enemy);
        return enemy;
    }

    // CO-1: Control Statement - Switch-Case for difficulty multiplier
    public int getDifficultyMultiplier(final String difficulty) {
        final String diff = (difficulty != null) ? difficulty.trim().toUpperCase() : "NORMAL";
        switch (diff) {
            case "EASY":
                return 1;
            case "HARD":
                return 3;
            case "NIGHTMARE":
                return 5;
            case "NORMAL":
            default:
                return 2;
        }
    }

    public void addEnemy(final Enemy enemy) {
        if (enemy != null) {
            enemies.add(enemy);
        }
    }

    public void spawnBoss() {
        enemies.add(new Boss(nextEnemyId++, "GigaDreadnought", 780.0, 300.0));
    }

    // CO-1: Looping - Counted For loop & CO-2: Polymorphic instantiation
    public void spawnWave(final int count) {
        for (int i = 0; i < count; ++i) {
            final double spawnX = 750.0 + (i * 60.0);
            final double spawnY = SPAWN_LANES[i % SPAWN_LANES.length];

            if (currentWave >= 2 && i % 3 == 0) {
                enemies.add(new Fighter(nextEnemyId++, "Fighter-" + (i + 1), spawnX, spawnY));
            } else {
                enemies.add(new Drone(nextEnemyId++, "Drone-" + (i + 1), spawnX, spawnY));
            }
        }

        if (currentWave % 3 == 0) {
            enemies.add(new Boss(nextEnemyId++, "Boss-W" + currentWave, 780.0, 300.0));
        }
    }

    // CO-1: Looping - Do-While loop
    public int spawnEnemiesBatch(final int count) {
        if (count <= 0) return 0;
        int spawned = 0;
        do {
            final double spawnX = 750.0 + (spawned * 40.0);
            final double spawnY = SPAWN_LANES[spawned % SPAWN_LANES.length];
            enemies.add(new Drone(nextEnemyId++, "BatchDrone-" + (spawned + 1), spawnX, spawnY));
            spawned++;
        } while (spawned < count);
        return spawned;
    }

    // CO-1: Looping - While loop
    public int advanceSimulationSteps(final int totalFrames) {
        int executedFrames = 0;
        while (executedFrames < totalFrames && running) {
            update();
            executedFrames++;
        }
        return executedFrames;
    }

    public void movePlayer(final double dx, final double dy) {
        if (!player.isAlive()) return;
        player.move(dx, dy, SCREEN_MIN_X + 20.0, SCREEN_MAX_X - 50.0, SCREEN_MIN_Y + 20.0, SCREEN_MAX_Y - 20.0);
    }

    public Bullet fireBullet() {
        if (!player.isAlive()) return null;
        final Bullet bullet = new Bullet(nextBulletId++, player.getX() + 30.0, player.getY(), 12.0, 0.0, 25, true);
        bullets.add(bullet);
        return bullet;
    }

    // CO-1: Looping - Enhanced For loop & CO-2: Lambda Expression (removeIf)
    public void update() {
        if (!running) return;

        for (final Bullet b : bullets) {
            b.update(SCREEN_MIN_X, SCREEN_MAX_X, SCREEN_MIN_Y, SCREEN_MAX_Y);
        }
        bullets.removeIf(b -> !b.isActive());

        for (final Enemy e : enemies) {
            e.move(); // CO-2: Runtime Polymorphism - Dynamic dispatch
        }
        enemies.removeIf(e -> !e.isActive());

        resolveCollisions();

        if (enemies.isEmpty()) {
            currentWave++;
            spawnWave(3 + currentWave);
        }

        if (!player.isAlive() && player.getLives() <= 0) {
            running = false;
        }
    }

    private void resolveCollisions() {
        final double bulletHitRadius = 15.0;
        final double playerHitRadius = 25.0;

        for (final Bullet b : bullets) {
            if (!b.isActive()) continue;

            for (final Enemy e : enemies) {
                if (!e.isActive()) continue;

                final double dx = Math.abs(b.getX() - e.getX());
                final double dy = Math.abs(b.getY() - e.getY());

                if (dx < bulletHitRadius && dy < bulletHitRadius) {
                    b.setActive(false);
                    e.takeDamage(b.getDamage());
                    if (!e.isAlive()) {
                        player.addScore(e.getScoreValue());
                    }
                    break;
                }
            }
        }

        for (final Enemy e : enemies) {
            if (!e.isActive()) continue;

            final double dx = Math.abs(e.getX() - player.getX());
            final double dy = Math.abs(e.getY() - player.getY());

            if (dx < playerHitRadius && dy < playerHitRadius) {
                e.setActive(false);
                player.takeDamage(e.getCollisionDamage());
            }
        }
    }

    public Player getPlayer() { return player; }
    public List<Enemy> getEnemies() { return enemies; }
    public List<Bullet> getBullets() { return bullets; }
    public boolean isRunning() { return running; }
    public int getCurrentWave() { return currentWave; }

    // CO-2: Lambda Expression - Comparator
    public void sortEnemiesByDistance() {
        enemies.sort((e1, e2) -> Double.compare(e1.getX(), e2.getX()));
    }

    // CO-2: Custom Functional Interface Lambda execution (EnemyAction)
    public void forEachActiveEnemy(final EnemyAction action) {
        if (action == null) return;
        for (final Enemy e : enemies) {
            if (e.isActive()) {
                action.execute(e);
            }
        }
    }

    // CO-2: Custom Functional Interface Lambda execution with return value (ScoreCalculator)
    public int calculateComboBonus(final int comboStreak, final ScoreCalculator calculator) {
        if (calculator == null) return 0;
        final int bonus = calculator.calculate(player.getScore(), comboStreak);
        player.addScore(bonus);
        return bonus;
    }

    // CO-1: StringBuffer for thread-safe mutable string construction
    public String getMissionSummary() {
        final StringBuffer sb = new StringBuffer(128);
        sb.append("MISSION STATUS: ").append(running ? "ACTIVE" : "GAME OVER");
        sb.append(" | PILOT: ").append(player.getName());
        sb.append(" | WAVE: ").append(currentWave);
        sb.append(" | SCORE: ").append(player.getScore());
        return sb.toString();
    }

    public GameState getGameState() {
        final GameState state = new GameState();
        state.setPlayerX(player.getX());
        state.setPlayerY(player.getY());
        state.setHealth(player.getHealth());
        state.setShield(player.getShield());
        state.setScore(player.getScore());
        state.setLives(player.getLives());
        state.setWave(currentWave);
        state.setGameOver(!running);
        state.setActiveBulletsCount(bullets.size());
        state.setActiveEnemiesCount(enemies.size());
        return state;
    }
}
