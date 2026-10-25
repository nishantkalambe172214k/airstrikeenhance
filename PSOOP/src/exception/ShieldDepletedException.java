package exception;

/**
 * Thrown when an energy shield absorption operation fails due to total shield depletion
 * or when a shield generator boost is commanded with 0 reserve energy.
 */
public class ShieldDepletedException extends GameException {
    private final int requestedAbsorption;
    private final int remainingShield;

    public ShieldDepletedException(final int requestedAbsorption, final int remainingShield) {
        super("ERR_SHIELD_DEPLETED",
              String.format("Deflector shields exhausted! Requested absorption: %d, Available: %d. Hull exposed to direct impact.",
                            requestedAbsorption, remainingShield));
        this.requestedAbsorption = requestedAbsorption;
        this.remainingShield = remainingShield;
    }

    public int getRequestedAbsorption() {
        return requestedAbsorption;
    }

    public int getRemainingShield() {
        return remainingShield;
    }
}
