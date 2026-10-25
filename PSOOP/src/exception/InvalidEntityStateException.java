package exception;

/**
 * Unchecked runtime exception thrown when an entity is initialized or modified
 * with invalid invariants (e.g., negative health bounds, NaN coordinates, or illegal IDs).
 */
public class InvalidEntityStateException extends RuntimeException {
    private final String entityId;

    public InvalidEntityStateException(final String message) {
        this("UNKNOWN_ENTITY", message);
    }

    public InvalidEntityStateException(final String entityId, final String message) {
        super(String.format("Invalid state for entity [%s]: %s", entityId, message));
        this.entityId = entityId;
    }

    public String getEntityId() {
        return entityId;
    }
}
