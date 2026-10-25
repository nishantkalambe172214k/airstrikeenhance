package model;

/**
 * Data model representing a procedural squadron wave element.
 * Designed for FIFO queueing in Queue<WaveSquad> within GameEngine.
 */
public class WaveSquad {
    private final int waveNumber;
    private final String enemyType;
    private final int count;
    private final double initialY;
    private final double ySpacing;

    public WaveSquad(final int waveNumber, final String enemyType, final int count,
                     final double initialY, final double ySpacing) {
        this.waveNumber = Math.max(1, waveNumber);
        this.enemyType = (enemyType != null) ? enemyType.toUpperCase() : "DRONE";
        this.count = Math.max(1, count);
        this.initialY = initialY;
        this.ySpacing = ySpacing;
    }

    public int getWaveNumber() {
        return waveNumber;
    }

    public String getEnemyType() {
        return enemyType;
    }

    public int count() {
        return count;
    }

    public int getCount() {
        return count;
    }

    public double getInitialY() {
        return initialY;
    }

    public double getYSpacing() {
        return ySpacing;
    }

    @Override
    public String toString() {
        return String.format("WaveSquad[W%d: %dx %s @ Y=%.1f]", waveNumber, count, enemyType, initialY);
    }
}
