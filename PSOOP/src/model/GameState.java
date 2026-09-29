package model;

public class GameState {
    private double playerX;
    private double playerY;
    private int health;
    private int shield;
    private int score;
    private int wave;
    private int ammo;
    private int lives;
    private boolean gameOver;
    private int activeBulletsCount;
    private int activeEnemiesCount;

    public GameState() {
        this.playerX = 100.0;
        this.playerY = 300.0;
        this.health = 100;
        this.shield = 50;
        this.score = 0;
        this.wave = 1;
        this.ammo = 999;
        this.lives = 3;
        this.gameOver = false;
        this.activeBulletsCount = 0;
        this.activeEnemiesCount = 0;
    }

    public double getPlayerX() { return playerX; }
    public void setPlayerX(double playerX) { this.playerX = playerX; }

    public double getPlayerY() { return playerY; }
    public void setPlayerY(double playerY) { this.playerY = playerY; }

    public int getHealth() { return health; }
    public void setHealth(int health) { this.health = health; }

    public int getShield() { return shield; }
    public void setShield(int shield) { this.shield = shield; }

    public int getScore() { return score; }
    public void setScore(int score) { this.score = score; }

    public int getWave() { return wave; }
    public void setWave(int wave) { this.wave = wave; }

    public int getAmmo() { return ammo; }
    public void setAmmo(int ammo) { this.ammo = ammo; }

    public int getLives() { return lives; }
    public void setLives(int lives) { this.lives = lives; }

    public boolean isGameOver() { return gameOver; }
    public void setGameOver(boolean gameOver) { this.gameOver = gameOver; }

    public int getActiveBulletsCount() { return activeBulletsCount; }
    public void setActiveBulletsCount(int activeBulletsCount) { this.activeBulletsCount = activeBulletsCount; }

    public int getActiveEnemiesCount() { return activeEnemiesCount; }
    public void setActiveEnemiesCount(int activeEnemiesCount) { this.activeEnemiesCount = activeEnemiesCount; }

    public String getHudDisplayText() {
        StringBuilder sb = new StringBuilder();
        sb.append("SCORE: ").append(score)
          .append(" | HP: ").append(health)
          .append(" | SHIELD: ").append(shield)
          .append(" | WAVE: ").append(wave)
          .append(" | LIVES: ").append(lives);
        return sb.toString();
    }

    @Override
    public String toString() {
        return String.format("[GAME STATE] Score=%d | HP=%d | Shield=%d | Wave=%d | Bullets=%d | Enemies=%d | Over=%b",
                score, health, shield, wave, activeBulletsCount, activeEnemiesCount, gameOver);
    }
}
