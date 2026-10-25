package exception;

/**
 * Base checked exception for the AirStriker PSOOP game engine.
 * Forms the root of all domain-specific checked exceptions.
 */
public class GameException extends Exception {
    private final String errorCode;

    public GameException(final String message) {
        this("ERR_GENERIC_GAME_EXCEPTION", message);
    }

    public GameException(final String errorCode, final String message) {
        super(message);
        this.errorCode = (errorCode != null) ? errorCode : "ERR_UNKNOWN";
    }

    public GameException(final String errorCode, final String message, final Throwable cause) {
        super(message, cause);
        this.errorCode = (errorCode != null) ? errorCode : "ERR_UNKNOWN";
    }

    public String getErrorCode() {
        return errorCode;
    }

    @Override
    public String toString() {
        return String.format("[%s] %s", errorCode, getMessage());
    }
}
