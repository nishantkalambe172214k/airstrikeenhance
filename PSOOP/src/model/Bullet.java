package model;

public class Bullet {
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

    public Bullet(int id, double x, double y, double vx, double vy, int damage, boolean playerBullet) {
        this.id = id;
        this.x = x;
        this.y = y;
        this.vx = vx;
        this.vy = vy;
        this.damage = Math.max(1, damage);
        this.active = true;
        this.playerBullet = playerBullet;
    }

    public void update(double minX, double maxX, double minY, double maxY) {
        if (!active) return;
        this.x += this.vx;
        this.y += this.vy;
        if (this.x < minX || this.x > maxX || this.y < minY || this.y > maxY) {
            this.active = false;
        }
    }

    public int getId() { return id; }
    public void setId(int id) { this.id = id; }

    public double getX() { return x; }
    public void setX(double x) { this.x = x; }

    public double getY() { return y; }
    public void setY(double y) { this.y = y; }

    public double getVx() { return vx; }
    public void setVx(double vx) { this.vx = vx; }

    public double getVy() { return vy; }
    public void setVy(double vy) { this.vy = vy; }

    public int getDamage() { return damage; }
    public void setDamage(int damage) { this.damage = Math.max(1, damage); }

    public boolean isActive() { return active; }
    public void setActive(boolean active) { this.active = active; }

    public boolean isPlayerBullet() { return playerBullet; }
    public void setPlayerBullet(boolean playerBullet) { this.playerBullet = playerBullet; }
}
