package model;

// CO-2: Multilevel Inheritance (Drone extends Enemy extends Character)
public class Drone extends Enemy {
    private double oscillationSpeed;
    private double oscillationAmplitude;
    private double initialY;

    public Drone() {
        this(100, "XenoDrone", 800.0, 300.0);
    }

    public Drone(final int id, final String name, final double startX, final double startY) {
        // CO-2: Super constructor call
        super(id, name, "DRONE", startX, startY, 30, 4.0, 100, 15);
        this.oscillationSpeed = 0.05;
        this.oscillationAmplitude = 15.0;
        this.initialY = startY;
    }

    public Drone(final int id, final String name, final double startX, final double startY,
                 final int maxHealth, final double speed, final int scoreValue, final int collisionDamage) {
        // CO-2: Super constructor call
        super(id, name, "DRONE", startX, startY, maxHealth, speed, scoreValue, collisionDamage);
        this.oscillationSpeed = 0.05;
        this.oscillationAmplitude = 15.0;
        this.initialY = startY;
    }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public void move() {
        setX(getX() - getSpeed());
        setY(initialY + Math.sin(getX() * oscillationSpeed) * oscillationAmplitude);
        if (getX() < -50.0) {
            setActive(false);
        }
    }

    public double getOscillationSpeed() { return oscillationSpeed; }
    public void setOscillationSpeed(final double oscillationSpeed) { this.oscillationSpeed = oscillationSpeed; }
    public double getOscillationAmplitude() { return oscillationAmplitude; }
    public void setOscillationAmplitude(final double oscillationAmplitude) { this.oscillationAmplitude = oscillationAmplitude; }
    public double getInitialY() { return initialY; }
    public void setInitialY(final double initialY) { this.initialY = initialY; }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[DRONE: %s] Pos=(%.1f, %.1f) HP=%d Spd=%.1f",
                getName(), getX(), getY(), getHealth(), getSpeed());
    }
}
