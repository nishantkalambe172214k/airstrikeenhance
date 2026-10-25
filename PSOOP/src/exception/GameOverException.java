package exception;

/**
 * Thrown when an interactive player action (firing weapons, executing evasive maneuvers)
 * is attempted while the mission has already concluded in a Game Over state.
 */
public class GameOverException extends GameException {
    private final int finalScore;
    private final int finalWave;

    public GameOverException(final int finalScore, final int finalWave) {
        super("ERR_GAME_OVER",
              String.format("Action rejected: Mission is terminated (Game Over). Final Score: %d, Wave: %d. Restart required.",
                            finalScore, finalWave));
        this.finalScore = finalScore;
        this.finalWave = finalWave;
    }

    public int getFinalScore() {
        return finalScore;
    }

    public int getFinalWave() {
        return finalWave;
    }
}
