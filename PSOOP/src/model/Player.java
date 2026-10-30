package model;

import exception.GameOverException;
import exception.ShieldDepletedException;
import exception.WeaponOverheatException;

// ============================================================================
// CO-2: Inheritance & Encapsulation (Player extends Character)
// CO-3: Combat System - Player Starship Entity
//       Implements the Movable interface contract, multi-layered shield/hull
//       combat damage resolution, bounded combat kinematics, score bounty tracking,
//       and checked weapon discharge mechanics.
// ============================================================================
public class Player extends Character {
    public static final double MAX_WEAPON_HEAT = 100.0;
    public static final double HEAT_PER_SHOT = 15.0;
    public static final double COOLING_RATE = 20.0; // Heat units per second

    private int score;
    private int shield;
    private int lives;
    private double weaponHeat;
    private int ammo;

    public Player() {
        this(1, "StarViper", 100.0, 300.0);
    }

    public Player(final int id, final String name, final double startX, final double startY) {
        // CO-2: Super constructor call to Character base class
        super(id, name, startX, startY, 100, 6.0);
        this.score = 0;
        this.shield = 50;  // CO-3: Combat System - Energy Deflector Shield capacity
        this.lives = 3;   // CO-3: Combat System - Hull reserve respawn tokens
        this.weaponHeat = 0.0; // CO-3/CO-4: Combat Thermal Management
        this.ammo = 999;
    }

    // ============================================================================
    // CO-3: Combat System - Interface Implementation (Movable)
    // Dynamic Polymorphism: Concrete implementation of Movable.move() for Player.
    // ============================================================================
    @Override
    public void move() {
        // Default stationary drift; active flight is managed via bounded overloaded move()
    }

    // ============================================================================
    // CO-3: Combat System - Compile-time Polymorphism (Method Overloading)
    // Bounded Combat Flight Kinematics:
    // Calculates delta translation based on directional vector (dx, dy) scaled
    // by speed, strictly clamping the starship within safe viewport combat boundaries
    // [minX..maxX, minY..maxY] to prevent player leaving the combat zone.
    // ============================================================================
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

    // ============================================================================
    // CO-3: Combat System - Combat Bounty Reward & Score Accumulation
    // Awards points upon defeating hostiles or sustaining combat combos.
    // ============================================================================
    public void addScore(final int points) {
        if (points > 0) {
            this.score += points;
        }
    }

    // ============================================================================
    // CO-3: Combat System - Dynamic Polymorphism (Method Overriding & Super Delegation)
    // Multi-Tier Combat Damage Resolution:
    // 1. Deflector Shield Interception: Incoming damage is first absorbed by the shield.
    // 2. Residual Armor Penetration: Any damage exceeding shield capacity is dispatched
    //    to the base Character class via super.takeDamage(remainingDamage).
    // 3. Emergency Combat Protocol: Upon fatal hull breach, consumes a life token,
    //    restores hull integrity to maximum, and recharges emergency shields to 25.
    // ============================================================================
    @Override
    public int takeDamage(final int amount) {
        if (amount <= 0)
            return 0;

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

        // CO-2 & CO-3: Super method delegation to Character base hull
        if (remainingDamage > 0) {
            super.takeDamage(remainingDamage);
        }

        if (!isAlive()) {
            if (this.lives > 1) {
                this.lives--;
                setHealth(getMaxHealth());
                this.shield = 25;
                setActive(true);
            } else {
                this.lives = 0;
                setActive(false);
            }
        }

        return amount;
    }

    public int getScore() {
        return score;
    }

    public void setScore(final int score) {
        this.score = Math.max(0, score);
    }

    public int getShield() {
        return shield;
    }

    public void setShield(final int shield) {
        this.shield = Math.max(0, shield);
    }

    public int getLives() {
        return lives;
    }

    public void setLives(final int lives) {
        this.lives = Math.max(0, lives);
    }

    // ============================================================================
    // CO-3: Combat System & CO-4: Custom Exception Handling
    // Controlled Combat Weapon Discharge:
    // Validates starship combat status against fatal game-over and weapon thermal
    // overheating limits. Throws GameOverException if hull is destroyed with zero lives,
    // and throws WeaponOverheatException if thermal threshold MAX_WEAPON_HEAT is exceeded.
    // Generates and returns a forward-moving high-velocity Bullet projectile upon firing.
    // ============================================================================
    public Bullet fireWeaponChecked(final int bulletId) throws WeaponOverheatException, GameOverException {
        if (!isAlive() && lives <= 0) {
            throw new GameOverException(score, 1);
        }
        if (ammo <= 0) {
            throw new WeaponOverheatException(weaponHeat, MAX_WEAPON_HEAT);
        }
        if (weaponHeat + HEAT_PER_SHOT > MAX_WEAPON_HEAT) {
            throw new WeaponOverheatException(weaponHeat, MAX_WEAPON_HEAT);
        }

        weaponHeat += HEAT_PER_SHOT;
        ammo--;
        return new Bullet(bulletId, getX() + 30.0, getY(), 12.0, 0.0, 25, true);
    }

    // ============================================================================
    // CO-3: Combat System & CO-4: Custom Exception Handling
    // Defensive Shield Absorption:
    // Verifies deflector field integrity. Throws ShieldDepletedException if shield
    // generator is drained (shield == 0), preventing silent defense failures.
    // ============================================================================
    public int absorbShieldChecked(final int damageAmount) throws ShieldDepletedException {
        if (this.shield <= 0) {
            throw new ShieldDepletedException(damageAmount, 0);
        }
        int absorbed = Math.min(this.shield, damageAmount);
        this.shield -= absorbed;
        return absorbed;
    }

    // ============================================================================
    // CO-3: Combat System - Weapon Heat Dissipation
    // Real-time animation cycle cooling down accumulated thermal energy by COOLING_RATE.
    // ============================================================================
    public void coolWeapon(final double deltaSeconds) {
        if (weaponHeat > 0.0) {
            weaponHeat = Math.max(0.0, weaponHeat - (COOLING_RATE * deltaSeconds));
        }
    }

    public double getWeaponHeat() {
        return weaponHeat;
    }

    public void setWeaponHeat(final double weaponHeat) {
        this.weaponHeat = Math.max(0.0, Math.min(MAX_WEAPON_HEAT, weaponHeat));
    }

    public boolean isOverheated() {
        return weaponHeat >= MAX_WEAPON_HEAT;
    }

    public int getAmmo() {
        return ammo;
    }

    public void setAmmo(final int ammo) {
        this.ammo = Math.max(0, ammo);
    }

    // CO-2: Runtime Polymorphism - Method Overriding
    @Override
    public String toString() {
        return String.format("[PLAYER: %s] Pos=(%.1f, %.1f) HP=%d Shield=%d Heat=%.1f Score=%d Lives=%d",
                getName(), getX(), getY(), getHealth(), shield, weaponHeat, score, lives);
    }
}
