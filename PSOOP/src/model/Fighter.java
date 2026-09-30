package model;

// CO-2: Multilevel Inheritance (Fighter extends Enemy extends Character)
public class Fighter extends Enemy {
    private int weaponDamage;
    private boolean evasionMode;

    public Fighter() {
        this(200, "XenoFighter", 800.0, 300.0);
    }

    public Fighter(final int id, final String name, final double startX, final double startY) {
        // CO-2: Super constructor call
        super(id, name, "FIGHTER", startX, startY, 70, 2.5, 250, 30);
        this.weaponDamage = 20;
        this.evasionMode = false;
    }

    public Fighter(final int id, final String name, final double startX, final double startY,
                   final int maxHealth, final double speed, final int scoreValue,
                   final int collisionDamage, final int weaponDamage) {
        // CO-2: Super constructor call
        super(id, name, "FIGHTER", startX, startY, maxHealth, speed, scoreValue, collisionDamage);
        this.weaponDamage = Math.max(1, weaponDamage);
        this.evasionMode = false;
    }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public void move() {
        setX(getX() - getSpeed());
        if (evasionMode) {
            setY(getY() + 1.5);
            if (getY() > 500.0) evasionMode = false;
        } else {
            setY(getY() - 1.5);
            if (getY() < 100.0) evasionMode = true;
        }

        if (getX() < -50.0) {
            setActive(false);
        }
    }

    public int getWeaponDamage() { return weaponDamage; }
    public void setWeaponDamage(final int weaponDamage) { this.weaponDamage = Math.max(1, weaponDamage); }
    public boolean isEvasionMode() { return evasionMode; }
    public void setEvasionMode(final boolean evasionMode) { this.evasionMode = evasionMode; }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[FIGHTER: %s] Pos=(%.1f, %.1f) HP=%d WpnDmg=%d",
                getName(), getX(), getY(), getHealth(), weaponDamage);
    }
}
