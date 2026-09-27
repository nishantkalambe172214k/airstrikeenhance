package model;

public class Player extends Character {
    private int score;
    private int shield;
    private int lives;

    public Player() {
        this(1, "StarViper", 100.0, 300.0);
    }

    public Player(int id, String name, double startX, double startY) {
        super(id, name, startX, startY, 100, 6.0);
        this.score = 0;
        this.shield = 50;
        this.lives = 3;
    }

    @Override
    public void move() {
    }

    public void move(double dx, double dy, double minX, double maxX, double minY, double maxY) {
        double newX = getX() + (dx * getSpeed());
        double newY = getY() + (dy * getSpeed());

        newX = Math.max(minX, Math.min(maxX, newX));
        newY = Math.max(minY, Math.min(maxY, newY));

        setX(newX);
        setY(newY);
    }

    public void addScore(int points) {
        if (points > 0) {
            this.score += points;
        }
    }

    @Override
    public int takeDamage(int amount) {
        if (amount <= 0) return 0;

        int remainingDamage = amount;
        if (this.shield > 0) {
            if (this.shield >= remainingDamage) {
                this.shield -= remainingDamage;
                remainingDamage = 0;
            } else {
                remainingDamage -= this.shield;
                this.shield = 0;
            }
        }

        if (remainingDamage > 0) {
            super.takeDamage(remainingDamage);
        }

        if (!isAlive() && this.lives > 1) {
            this.lives--;
            setHealth(getMaxHealth());
            this.shield = 25;
            setActive(true);
        }

        return amount;
    }

    public int getScore() { return score; }
    public void setScore(int score) { this.score = Math.max(0, score); }

    public int getShield() { return shield; }
    public void setShield(int shield) { this.shield = Math.max(0, shield); }

    public int getLives() { return lives; }
    public void setLives(int lives) { this.lives = Math.max(0, lives); }

    @Override
    public String toString() {
        return String.format("[PLAYER: %s] Pos=(%.1f, %.1f) HP=%d Shield=%d Score=%d Lives=%d",
                getName(), getX(), getY(), getHealth(), shield, score, lives);
    }
}
