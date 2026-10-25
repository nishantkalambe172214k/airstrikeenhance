package model;

/**
 * Domain model representing a pilot's profile, historical high score,
 * and mission telemetry. Implements Comparable for natural sorting in Collections.
 */
public class PilotProfile implements Comparable<PilotProfile> {
    private int playerId;
    private String callsign;
    private int gamesPlayed;
    private int highScore;
    private int highestWave;
    private String createdAt;

    public PilotProfile(final int playerId, final String callsign, final int gamesPlayed,
                        final int highScore, final int highestWave, final String createdAt) {
        this.playerId = playerId;
        this.callsign = (callsign != null && !callsign.trim().isEmpty()) ? callsign.trim() : "UnknownPilot";
        this.gamesPlayed = Math.max(0, gamesPlayed);
        this.highScore = Math.max(0, highScore);
        this.highestWave = Math.max(1, highestWave);
        this.createdAt = (createdAt != null) ? createdAt : "2026-01-01";
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(final int playerId) {
        this.playerId = playerId;
    }

    public String getCallsign() {
        return callsign;
    }

    public void setCallsign(final String callsign) {
        if (callsign != null && !callsign.trim().isEmpty()) {
            this.callsign = callsign.trim();
        }
    }

    public int getGamesPlayed() {
        return gamesPlayed;
    }

    public void incrementGamesPlayed() {
        this.gamesPlayed++;
    }

    public int getHighScore() {
        return highScore;
    }

    public boolean updateHighScore(final int newScore) {
        if (newScore > this.highScore) {
            this.highScore = newScore;
            return true;
        }
        return false;
    }

    public int getHighestWave() {
        return highestWave;
    }

    public void setHighestWave(final int wave) {
        if (wave > this.highestWave) {
            this.highestWave = wave;
        }
    }

    public String getCreatedAt() {
        return createdAt;
    }

    /**
     * Natural order comparison: sorts by highScore in descending order
     * for automatic leaderboard ranking using Collections.sort().
     */
    @Override
    public int compareTo(final PilotProfile other) {
        if (other == null) return -1;
        // Descending order of high score
        int scoreCompare = Integer.compare(other.highScore, this.highScore);
        if (scoreCompare != 0) {
            return scoreCompare;
        }
        return this.callsign.compareToIgnoreCase(other.callsign);
    }

    @Override
    public String toString() {
        return String.format("[%d] %-15s Score: %-6d | Wave: %-2d | Battles: %-3d",
                playerId, callsign, highScore, highestWave, gamesPlayed);
    }
}
