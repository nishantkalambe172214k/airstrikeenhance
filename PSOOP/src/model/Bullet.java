package model;

// CO-2: Final Class (cannot be extended)
public final class Bullet {
    // CO-1: Encapsulation (private fields)
    private int id;
    private double x;
    private double y;
    private double vx;
    private double vy;
    private int damage;
    private boolean active;
    private boolean playerBullet;

    public Bullet() {
        this(0, 0.0, 0.0, 10.0, 0.0, 25, true);
    }

    // CO-1: Parameterized Constructor & CO-2: Final parameters
    public Bullet(final int id, final double x, final double y,
                  final double vx, final double vy,
                  final int damage, final boolean playerBullet) {
        this.id = id;
        this.x = x;
        this.y = y;
        this.vx = vx;
        this.vy = vy;
        this.damage = Math.max(1, damage);
        this.active = true;
        this.playerBullet = playerBullet;
    }

    // CO-1: Method with final parameters
    public void update(final double minX, final double maxX, final double minY, final double maxY) {
        if (!active) return;
        this.x += this.vx;
        this.y += this.vy;
        if (this.x < minX || this.x > maxX || this.y < minY || this.y > maxY) {
            this.active = false;
        }
    }

    // CO-1: Getters and Setters
    public int getId() { return id; }
    public void setId(final int id) { this.id = id; }
    public double getX() { return x; }
    public void setX(final double x) { this.x = x; }
    public double getY() { return y; }
    public void setY(final double y) { this.y = y; }
    public double getVx() { return vx; }
    public void setVx(final double vx) { this.vx = vx; }
    public double getVy() { return vy; }
    public void setVy(final double vy) { this.vy = vy; }
    public int getDamage() { return damage; }
    public void setDamage(final int damage) { this.damage = Math.max(1, damage); }
    public boolean isActive() { return active; }
    public void setActive(final boolean active) { this.active = active; }
    public boolean isPlayerBullet() { return playerBullet; }
    public void setPlayerBullet(final boolean playerBullet) { this.playerBullet = playerBullet; }
}
