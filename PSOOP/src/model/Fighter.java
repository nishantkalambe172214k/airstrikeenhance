package model;

public class Fighter extends Enemy {
    private int weaponDamage;
    private boolean evasionMode;

    public Fighter() {
        this(200, "XenoFighter", 800.0, 300.0);
    }

    public Fighter(int id, String name, double startX, double startY) {
        super(id, name, "FIGHTER", startX, startY, 70, 2.5, 250, 30);
        this.weaponDamage = 20;
        this.evasionMode = false;
    }

    public Fighter(int id, String name, double startX, double startY,
                   int maxHealth, double speed, int scoreValue, int collisionDamage, int weaponDamage) {
        super(id, name, "FIGHTER", startX, startY, maxHealth, speed, scoreValue, collisionDamage);
        this.weaponDamage = Math.max(1, weaponDamage);
        this.evasionMode = false;
    }

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
    public void setWeaponDamage(int weaponDamage) { this.weaponDamage = Math.max(1, weaponDamage); }

    public boolean isEvasionMode() { return evasionMode; }
    public void setEvasionMode(boolean evasionMode) { this.evasionMode = evasionMode; }

    @Override
    public String toString() {
        return String.format("[FIGHTER: %s] Pos=(%.1f, %.1f) HP=%d WpnDmg=%d",
                getName(), getX(), getY(), getHealth(), weaponDamage);
    }
}
