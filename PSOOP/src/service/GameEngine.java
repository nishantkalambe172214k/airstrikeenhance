package service;

import model.Bullet;
import model.Enemy;
import model.GameState;
import model.Player;

import java.util.ArrayList;
import java.util.List;

public class GameEngine {
    public static final double SCREEN_MIN_X = 0.0;
    public static final double SCREEN_MAX_X = 800.0;
    public static final double SCREEN_MIN_Y = 0.0;
    public static final double SCREEN_MAX_Y = 600.0;

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

    public GameEngine(String pilotName) {
        this.player = new Player(1, pilotName, 100.0, 300.0);
        this.enemies = new ArrayList<>();
        this.bullets = new ArrayList<>();
        this.nextBulletId = 1;
        this.nextEnemyId = 100;
        this.currentWave = 1;
        this.running = true;
    }

    public void spawnWave(int count) {
        for (int i = 0; i < count; ++i) {
            double spawnX = 750.0 + (i * 60.0);
            double spawnY = 100.0 + ((i * 110.0) % 400.0);
            enemies.add(new Enemy(nextEnemyId++, "Drone-" + (i + 1), "DRONE",
                    spawnX, spawnY, 40, 2.5, 100, 20));
        }
    }

    public void movePlayer(double dx, double dy) {
        if (!player.isAlive()) return;
        player.move(dx, dy, SCREEN_MIN_X + 20.0, SCREEN_MAX_X - 50.0, SCREEN_MIN_Y + 20.0, SCREEN_MAX_Y - 20.0);
    }

    public Bullet fireBullet() {
        if (!player.isAlive()) return null;
        Bullet bullet = new Bullet(nextBulletId++, player.getX() + 30.0, player.getY(), 12.0, 0.0, 25, true);
        bullets.add(bullet);
        return bullet;
    }

    public void update() {
        if (!running) return;

        for (Bullet b : bullets) {
            b.update(SCREEN_MIN_X, SCREEN_MAX_X, SCREEN_MIN_Y, SCREEN_MAX_Y);
        }
        bullets.removeIf(b -> !b.isActive());

        for (Enemy e : enemies) {
            e.move();
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
        double bulletHitRadius = 15.0;
        double playerHitRadius = 25.0;

        for (Bullet b : bullets) {
            if (!b.isActive()) continue;

            for (Enemy e : enemies) {
                if (!e.isActive()) continue;

                double dx = Math.abs(b.getX() - e.getX());
                double dy = Math.abs(b.getY() - e.getY());

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

        for (Enemy e : enemies) {
            if (!e.isActive()) continue;

            double dx = Math.abs(e.getX() - player.getX());
            double dy = Math.abs(e.getY() - player.getY());

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

    public GameState getGameState() {
        GameState state = new GameState();
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
