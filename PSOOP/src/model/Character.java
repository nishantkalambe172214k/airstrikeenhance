package model;

// CO-2: Abstract Class implementing Interface
public abstract class Character implements Movable {
    // CO-1: Encapsulation (private fields)
    private int id;
    private String name;
    private double x;
    private double y;
    private int health;
    private int maxHealth;
    private double speed;
    private boolean active;

    // CO-1: Default Constructor using 'this' keyword constructor chaining
    public Character() {
        this(0, "Unknown", 0.0, 0.0, 100, 5.0);
    }

    // CO-1: Parameterized Constructor
    public Character(int id, String name, double x, double y, int maxHealth, double speed) {
        this.id = id;
        this.name = (name != null && !name.trim().isEmpty()) ? name.trim() : "Unknown";
        this.x = x;
        this.y = y;
        this.maxHealth = Math.max(1, maxHealth);
        this.health = this.maxHealth;
        this.speed = Math.max(0, speed);
        this.active = true;
    }

    // CO-1: Method with Control Statements (if-else)
    public int takeDamage(int amount) {
        if (amount <= 0) return 0;
        int damageDealt = Math.min(this.health, amount);
        this.health -= damageDealt;
        if (this.health == 0) {
            this.active = false;
        }
        return damageDealt;
    }

    // CO-1: Method
    public void heal(int amount) {
        if (amount <= 0) return;
        this.health = Math.min(this.maxHealth, this.health + amount);
        if (this.health > 0) {
            this.active = true;
        }
    }

    // CO-2: Final Methods (cannot be overridden by subclasses)
    public final boolean isAlive() {
        return this.active && this.health > 0;
    }

    public final int getId() {
        return id;
    }

    public final int getMaxHealth() {
        return maxHealth;
    }

    public final boolean isActive() {
        return active;
    }

    // CO-2: Abstract Method (subclasses must implement)
    @Override
    public abstract void move();

    // CO-1: Getters and Setters
    public void setId(int id) { this.id = id; }
    public void setActive(boolean active) { this.active = active; }
    public String getName() { return name; }
    public void setName(String name) {
        if (name != null && !name.trim().isEmpty()) {
            this.name = name.trim();
        }
    }
    public double getX() { return x; }
    public void setX(double x) { this.x = x; }
    public double getY() { return y; }
    public void setY(double y) { this.y = y; }
    public int getHealth() { return health; }
    public void setHealth(int health) {
        this.health = Math.max(0, Math.min(this.maxHealth, health));
        if (this.health == 0) {
            this.active = false;
        }
    }
    public double getSpeed() { return speed; }
    public void setSpeed(double speed) { this.speed = Math.max(0, speed); }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[%s #%d] Pos=(%.1f, %.1f) HP=%d/%d Spd=%.1f",
                name, id, x, y, health, maxHealth, speed);
    }
}
