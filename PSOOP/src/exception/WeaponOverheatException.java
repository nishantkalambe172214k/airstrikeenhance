package exception;

/**
 * Thrown when the player attempts to fire while the weapon system heat
 * level has reached or exceeded critical threshold (100%), or when ammo is depleted.
 */
public class WeaponOverheatException extends GameException {
    private final double currentHeat;
    private final double maxHeat;

    public WeaponOverheatException(final double currentHeat, final double maxHeat) {
        super("ERR_WEAPON_OVERHEAT",
              String.format("Plasma cannon overheated! Heat level: %.1f / %.1f. Cooling required before firing.",
                            currentHeat, maxHeat));
        this.currentHeat = currentHeat;
        this.maxHeat = maxHeat;
    }

    public double getCurrentHeat() {
        return currentHeat;
    }

    public double getMaxHeat() {
        return maxHeat;
    }
}
