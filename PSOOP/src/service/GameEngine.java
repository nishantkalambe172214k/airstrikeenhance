package service;

import exception.GameException;
import exception.GameOverException;
import exception.InvalidEntityStateException;
import exception.WeaponOverheatException;
import model.Boss;
import model.Bullet;
import model.Drone;
import model.Enemy;
import model.EnemyAction;
import model.Fighter;
import model.GameState;
import model.Player;
import model.ScoreCalculator;
import model.WaveSquad;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

public class GameEngine {
    // CO-2: Static Final constants
    public static final double SCREEN_MIN_X = 0.0;
    public static final double SCREEN_MAX_X = 800.0;
    public static final double SCREEN_MIN_Y = 0.0;
    public static final double SCREEN_MAX_Y = 600.0;

    // CO-1: 1D Array for spatial lanes
    private static final double[] SPAWN_LANES = { 100.0, 200.0, 300.0, 400.0, 500.0 };

    // Java Collections Framework:
    // List: bullets, enemies
    // Map: enemyMap (O(1) ID lookup)
    // Set: destroyedEnemyIds (ensures unique enemy elimination accounting)
    // Queue: waveQueue (FIFO squad deployment pipeline)
    // Deque: combatEventLog (rolling combat telemetry history)
    private static final int MAX_LOG_CAPACITY = 20;

    private Player player;
    private List<Enemy> enemies;
    private List<Bullet> bullets;
    private final Map<Integer, Enemy> enemyMap;
    private final Set<Integer> destroyedEnemyIds;
    private final Queue<WaveSquad> waveQueue;
    private final Deque<String> combatEventLog;

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
        this.enemyMap = new HashMap<>();
        this.destroyedEnemyIds = new HashSet<>();
        this.waveQueue = new LinkedList<>();
        this.combatEventLog = new ArrayDeque<>();
        this.nextBulletId = 1;
        this.nextEnemyId = 100;
        this.currentWave = 1;
        this.running = true;
        logEvent("[SYSTEM] AirStriker mission initialized for Pilot: " + player.getName());
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
        enemyMap.put(enemy.getId(), enemy);
        return enemy;
    }

    // Exception Handling: Validation method throwing unchecked InvalidEntityStateException
    public Enemy spawnEnemyByTypeValidated(final String enemyType, final double x, final double y)
            throws InvalidEntityStateException {
        if (x < -150.0 || x > SCREEN_MAX_X + 600.0 || y < -50.0 || y > SCREEN_MAX_Y + 50.0) {
            throw new InvalidEntityStateException("SPAWN_LOC",
                    String.format("Target coordinate (%.1f, %.1f) is out of spawn sector bounds.", x, y));
        }
        return spawnEnemyByType(enemyType, x, y);
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
            enemyMap.put(enemy.getId(), enemy);
        }
    }

    public void spawnBoss() {
        Boss boss = new Boss(nextEnemyId++, "GigaDreadnought", 780.0, 300.0);
        enemies.add(boss);
        enemyMap.put(boss.getId(), boss);
    }

    // Wave Spawning
    // SR11

    public void spawnWave(final int count) {
        for (int i = 0; i < count; ++i) {
            final double spawnX = 750.0 + (i * 60.0);
            final double spawnY = SPAWN_LANES[i % SPAWN_LANES.length];

            Enemy enemy;
            if (currentWave >= 2 && i % 3 == 0) {
                enemy = new Fighter(nextEnemyId++, "Fighter-" + (i + 1), spawnX, spawnY);
            } else {
                enemy = new Drone(nextEnemyId++, "Drone-" + (i + 1), spawnX, spawnY);
            }
            enemies.add(enemy);
            enemyMap.put(enemy.getId(), enemy);
        }

        if (currentWave % 3 == 0) {
            Boss boss = new Boss(nextEnemyId++, "Boss-W" + currentWave, 780.0, 300.0);
            enemies.add(boss);
            enemyMap.put(boss.getId(), boss);
        }
    }

    // Java Collections Framework: Queue<WaveSquad> FIFO deployment pipeline
    public void enqueueWaveSquad(final WaveSquad squad) {
        if (squad != null) {
            waveQueue.offer(squad);
            logEvent(String.format("[QUEUE] Wave squad registered: %s", squad));
        }
    }

    public WaveSquad pollNextSquad() {
        return waveQueue.poll();
    }

    public boolean deployNextWaveSquad() {
        WaveSquad squad = waveQueue.poll();
        if (squad == null) return false;

        logEvent(String.format("[DEPLOY] Deploying %dx %s from wave queue", squad.getCount(), squad.getEnemyType()));
        for (int i = 0; i < squad.getCount(); i++) {
            double spawnX = 750.0 + (i * 50.0);
            double spawnY = squad.getInitialY() + (i * squad.getYSpacing());
            spawnEnemyByType(squad.getEnemyType(), spawnX, spawnY);
        }
        return true;
    }

    // CO-1: Looping - Do-While loop
    // SR11

    public int spawnEnemiesBatch(final int count) {
        if (count <= 0)
            return 0;
        int spawned = 0;
        do {
            final double spawnX = 750.0 + (spawned * 40.0);
            final double spawnY = SPAWN_LANES[spawned % SPAWN_LANES.length];
            enemies.add(new Drone(nextEnemyId++, "BatchDrone-" + (spawned + 1), spawnX, spawnY));
            spawned++;
        } while (spawned < count);
        return spawned;
    }

    // Frame Simulation
    public int advanceSimulationSteps(final int totalFrames) {
        int executedFrames = 0;
        while (executedFrames < totalFrames && running) {
            update();
            executedFrames++;
        }
        return executedFrames;
    }

    // Exception Handling: Robust simulation method demonstrating try-catch-finally
    public int safeAdvanceSimulation(final int totalFrames) throws GameException {
        if (totalFrames < 0) {
            throw new GameException("ERR_INVALID_FRAMES", "Simulation step count cannot be negative: " + totalFrames);
        }
        int executedFrames = 0;
        try {
            while (executedFrames < totalFrames && running) {
                update();
                executedFrames++;
            }
        } catch (Exception e) {
            logEvent("[FATAL] Simulation loop interrupted: " + e.getMessage());
            throw new GameException("ERR_SIM_FAULT", "Exception occurred during simulation step", e);
        } finally {
            // Guaranteed cleanup and state logging
            logEvent(String.format("[SIM_SYNC] Completed %d simulation frames. Enemies remaining: %d, Bullets: %d.",
                    executedFrames, enemies.size(), bullets.size()));
        }
        return executedFrames;
    }

    public void movePlayer(final double dx, final double dy) {
        if (!player.isAlive())
            return;
        player.move(dx, dy, SCREEN_MIN_X + 20.0, SCREEN_MAX_X - 50.0, SCREEN_MIN_Y + 20.0, SCREEN_MAX_Y - 20.0);
    }

    // Exception Handling: Checked weapon firing throwing custom exceptions
    public Bullet fireBulletChecked() throws WeaponOverheatException, GameOverException {
        if (!player.isAlive() && player.getLives() <= 0) {
            throw new GameOverException(player.getScore(), currentWave);
        }
        Bullet bullet = player.fireWeaponChecked(nextBulletId++);
        bullets.add(bullet);
        logEvent(String.format("[FIRE] Plasma bolt #%d launched by %s (Heat: %.1f%%)",
                bullet.getId(), player.getName(), player.getWeaponHeat()));
        return bullet;
    }

    // Compatible wrapper for fireBullet handling exceptions gracefully
    public Bullet fireBullet() {
        if (!player.isAlive())
            return null;
        try {
            return fireBulletChecked();
        } catch (WeaponOverheatException | GameOverException e) {
            logEvent("[WARN] " + e.getMessage());
            final Bullet bullet = new Bullet(nextBulletId++, player.getX() + 30.0, player.getY(), 12.0, 0.0, 25, true);
            bullets.add(bullet);
            return bullet;
        }
    }

    // SR11

    public void update() {
        if (!running)
            return;

        // Weapon heat cooling cycle (60 FPS delta)
        player.coolWeapon(1.0 / 60.0);

        // Java Collections: Iterator traversal over bullets with safe removal
        Iterator<Bullet> bulletIt = bullets.iterator();
        while (bulletIt.hasNext()) {
            Bullet b = bulletIt.next();
            b.update(SCREEN_MIN_X, SCREEN_MAX_X, SCREEN_MIN_Y, SCREEN_MAX_Y);
            if (!b.isActive()) {
                bulletIt.remove();
            }
        }

        // Deploy from waveQueue if active enemies are low
        if (!waveQueue.isEmpty() && enemies.size() < 3) {
            deployNextWaveSquad();
        }

        // Java Collections: Iterator traversal over polymorphic enemies
        Iterator<Enemy> enemyIt = enemies.iterator();
        while (enemyIt.hasNext()) {
            Enemy e = enemyIt.next();
            e.move(); // Dynamically dispatches to Drone.move(), Fighter.move(), or Boss.move()
            if (!e.isActive()) {
                if (!e.isAlive()) {
                    destroyedEnemyIds.add(e.getId());
                }
                enemyMap.remove(e.getId());
                enemyIt.remove();
            }
        }

        resolveCollisions();

        if (enemies.isEmpty() && waveQueue.isEmpty()) {
            currentWave++;
            logEvent("[WAVE] Advancing to Wave " + currentWave + "! Hostile squadron arriving.");
            spawnWave(3 + currentWave);
        }

        if (!player.isAlive() || player.getLives() <= 0) {
            player.setActive(false);
            player.setLives(0);
            running = false;
            logEvent(String.format("[GAME OVER] Mission terminated for Pilot %s. Final Score: %d",
                    player.getName(), player.getScore()));
        }
    }

    private void resolveCollisions() {
        final double bulletHitRadius = 15.0;
        final double playerHitRadius = 25.0;

        for (final Bullet b : bullets) {
            if (!b.isActive())
                continue;

            for (final Enemy e : enemies) {
                if (!e.isActive())
                    continue;

                final double dx = Math.abs(b.getX() - e.getX());
                final double dy = Math.abs(b.getY() - e.getY());

                if (dx < bulletHitRadius && dy < bulletHitRadius) {
                    b.setActive(false);
                    e.takeDamage(b.getDamage());
                    if (!e.isAlive()) {
                        // Java Collections: Set ensures unique tracking of defeated enemy IDs
                        destroyedEnemyIds.add(e.getId());
                        enemyMap.remove(e.getId());
                        player.addScore(e.getScoreValue());
                        logEvent(String.format("[HIT] Hostile %s #%d neutralized! (+%d PTS)",
                                e.getName(), e.getId(), e.getScoreValue()));
                    }
                    break;
                }
            }
        }

        for (final Enemy e : enemies) {
            if (!e.isActive())
                continue;

            final double dx = Math.abs(e.getX() - player.getX());
            final double dy = Math.abs(e.getY() - player.getY());

            if (dx < playerHitRadius && dy < playerHitRadius) {
                e.setActive(false);
                enemyMap.remove(e.getId());
                player.takeDamage(e.getCollisionDamage());
                logEvent(String.format("[COLLISION] Hull rammed by %s! Player HP: %d, Shield: %d",
                        e.getName(), player.getHealth(), player.getShield()));
            }
        }
    }

    public Player getPlayer() {
        return player;
    }

    public List<Enemy> getEnemies() {
        return enemies;
    }

    public List<Bullet> getBullets() {
        return bullets;
    }

    // Java Collections: Map lookup for enemy by ID (O(1))
    public Enemy getEnemyById(final int id) {
        return enemyMap.get(id);
    }

    public Map<Integer, Enemy> getEnemyMap() {
        return Collections.unmodifiableMap(enemyMap);
    }

    // Java Collections: Set of destroyed enemy IDs
    public Set<Integer> getDestroyedEnemyIds() {
        return Collections.unmodifiableSet(destroyedEnemyIds);
    }

    // Java Collections: Queue of incoming waves
    public Queue<WaveSquad> getWaveQueue() {
        return waveQueue;
    }

    // Java Collections: Deque of combat telemetry events
    public Deque<String> getCombatEventLog() {
        return combatEventLog;
    }

    public void logEvent(final String message) {
        if (message == null) return;
        if (combatEventLog.size() >= MAX_LOG_CAPACITY) {
            combatEventLog.pollFirst();
        }
        combatEventLog.offerLast(message);
    }

    public boolean isRunning() {
        return running;
    }

    public void setRunning(final boolean running) {
        this.running = running;
    }

    public int getCurrentWave() {
        return currentWave;
    }

    public void setCurrentWave(final int wave) {
        this.currentWave = Math.max(1, wave);
    }

    // CO-2: Lambda Expression - Comparator
    public void sortEnemiesByDistance() {
        enemies.sort((e1, e2) -> Double.compare(e1.getX(), e2.getX()));
    }

    // CO-2: Custom Functional Interface Lambda execution (EnemyAction)
    public void forEachActiveEnemy(final EnemyAction action) {
        if (action == null)
            return;
        for (final Enemy e : enemies) {
            if (e.isActive()) {
                action.execute(e);
            }
        }
    }

    // CO-2: Custom Functional Interface Lambda execution with return value
    // (ScoreCalculator)
    public int calculateComboBonus(final int comboStreak, final ScoreCalculator calculator) {
        if (calculator == null)
            return 0;
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
