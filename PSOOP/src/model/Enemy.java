package model;

// CO-2: Inheritance (Enemy extends Character)
public class Enemy extends Character {
    private String enemyType;
    private int scoreValue;
    private int collisionDamage;

    public Enemy() {
        this(100, "XenoDrone", "DRONE", 800.0, 300.0, 40, 3.0, 100, 20);
    }

    public Enemy(final int id, final String name, final String enemyType,
                 final double startX, final double startY,
                 final int maxHealth, final double speed,
                 final int scoreValue, final int collisionDamage) {
        // CO-2: Super constructor call
        super(id, name, startX, startY, maxHealth, speed);
        this.enemyType = (enemyType != null) ? enemyType.toUpperCase() : "DRONE";
        this.scoreValue = Math.max(0, scoreValue);
        this.collisionDamage = Math.max(0, collisionDamage);
    }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public void move() {
        setX(getX() - getSpeed());
        if (getX() < -50.0) {
            setActive(false);
        }
    }

    public String getEnemyType() { return enemyType; }
    public void setEnemyType(final String enemyType) {
        this.enemyType = (enemyType != null) ? enemyType.toUpperCase() : "DRONE";
    }
    public int getScoreValue() { return scoreValue; }
    public void setScoreValue(final int scoreValue) { this.scoreValue = Math.max(0, scoreValue); }
    public int getCollisionDamage() { return collisionDamage; }
    public void setCollisionDamage(final int collisionDamage) { this.collisionDamage = Math.max(0, collisionDamage); }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[ENEMY: %s (%s)] Pos=(%.1f, %.1f) HP=%d ScoreVal=%d Dmg=%d",
                getName(), enemyType, getX(), getY(), getHealth(), scoreValue, collisionDamage);
    }
}
