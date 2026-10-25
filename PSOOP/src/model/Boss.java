package model;

// CO-2: Multilevel Inheritance (Boss extends Enemy extends Character)
public class Boss extends Enemy {
    private int armorRating;
    private int phase;
    private boolean enrageMode;

    public Boss() {
        this(500, "GigaDreadnought", 750.0, 300.0);
    }

    public Boss(final int id, final String name, final double startX, final double startY) {
        // CO-2: Super constructor call
        super(id, name, "BOSS", startX, startY, 300, 1.0, 1000, 60);
        this.armorRating = 5;
        this.phase = 1;
        this.enrageMode = false;
    }

    public Boss(final int id, final String name, final double startX, final double startY,
                final int maxHealth, final double speed, final int scoreValue,
                final int collisionDamage, final int armorRating) {
        // CO-2: Super constructor call
        super(id, name, "BOSS", startX, startY, maxHealth, speed, scoreValue, collisionDamage);
        this.armorRating = Math.
        max(0, armorRating);
        this.phase = 1;
        this.enrageMode = false;
    }

    // ============================================================================
    // CO-2 & CO-3: Dynamic Polymorphism & Super Method Delegation (super.takeDamage)
    // CO-3: Combat System - Armor Plate Mitigation & Phase 2 Enrage Transition
    // 1. Armor Interception: incoming damage is reduced by armorRating (min 1 dmg).
    // 2. Hull Degradation: residual damage passed to super.takeDamage().
    // 3. Boss Phase Shift: When hull falls below 40%, enters Phase 2 Enrage mode,
    //    boosting combat speed by 1.5x.
    // ============================================================================
    @Override
    public int takeDamage(final int amount) {
        if (amount <= 0) return 0;
        int effectiveDamage = Math.max(1, amount - this.armorRating);
        int damageDealt = super.takeDamage(effectiveDamage);

        double healthPercent = (double) getHealth() / getMaxHealth();
        if (healthPercent <= 0.4 && !enrageMode) {
            this.enrageMode = true;
            this.phase = 2;
            setSpeed(getSpeed() * 1.5);
        }

        return damageDealt;
    }

    // ============================================================================
    // CO-2 & CO-3: Dynamic Polymorphism (Runtime Method Overriding)
    // CO-3: Combat System - Two-Phase Boss Combat Patrol Kinematics
    // Phase A: Advances onto battlefield until reaching battle station (X <= 600.0).
    // Phase B: Locks into a continuous vertical scanning patrol, accelerating in enrage mode.
    // ============================================================================
    @Override
    public void move() {
        if (getX() > 600.0) {
            setX(getX() - getSpeed());
        } else {
            setY(getY() + (enrageMode ? 2.5 : 1.5));
            if (getY() > 480.0) {
                setY(120.0);
            }
        }
    }

    public int getArmorRating() { return armorRating; }
    public void setArmorRating(final int armorRating) { this.armorRating = Math.max(0, armorRating); }
    public int getPhase() { return phase; }
    public void setPhase(final int phase) { this.phase = Math.max(1, phase); }
    public boolean isEnrageMode() { return enrageMode; }
    public void setEnrageMode(final boolean enrageMode) { this.enrageMode = enrageMode; }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[BOSS: %s] Pos=(%.1f, %.1f) HP=%d Armor=%d Phase=%d Enraged=%b",
                getName(), getX(), getY(), getHealth(), armorRating, phase, enrageMode);
    }
}
