package model;

// CO-2: Inheritance (Player extends Character)
public class Player extends Character {
    private int score;
    private int shield;
    private int lives;

    public Player() {
        this(1, "StarViper", 100.0, 300.0);
    }

    public Player(final int id, final String name, final double startX, final double startY) {
        // CO-2: Super constructor call
        super(id, name, startX, startY, 100, 6.0);
        this.score = 0;
        this.shield = 50;
        this.lives = 3;
    }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public void move() {
    }

    // CO-2: Compile-time Polymorphism - Method Overloading with final parameters
    public void move(final double dx, final double dy,
                     final double minX, final double maxX,
                     final double minY, final double maxY) {
        double newX = getX() + (dx * getSpeed());
        double newY = getY() + (dy * getSpeed());

        newX = Math.max(minX, Math.min(maxX, newX));
        newY = Math.max(minY, Math.min(maxY, newY));

        setX(newX);
        setY(newY);
    }

    public void addScore(final int points) {
        if (points > 0) {
            this.score += points;
        }
    }

    // CO-2: Runtime Polymorphism - Method Overriding & Super method call
    @Override
    public int takeDamage(final int amount) {
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

        // CO-2: Super method call
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
    public void setScore(final int score) { this.score = Math.max(0, score); }
    public int getShield() { return shield; }
    public void setShield(final int shield) { this.shield = Math.max(0, shield); }
    public int getLives() { return lives; }
    public void setLives(final int lives) { this.lives = Math.max(0, lives); }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[PLAYER: %s] Pos=(%.1f, %.1f) HP=%d Shield=%d Score=%d Lives=%d",
                getName(), getX(), getY(), getHealth(), shield, score, lives);
    }
}
