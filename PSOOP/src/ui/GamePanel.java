package ui;

import exception.GameOverException;
import exception.WeaponOverheatException;
import model.Boss;
import model.Bullet;
import model.Drone;
import model.Enemy;
import model.Fighter;
import model.Player;
import service.GameEngine;
import service.LeaderboardService;

import javax.swing.JPanel;
import javax.swing.Timer;
import java.awt.AlphaComposite;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Polygon;
import java.awt.RadialGradientPaint;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.Set;

/**
 * AirStriker Main Java Swing Game Canvas.
 * Demonstrates:
 * 1. Java Swing: Custom double-buffered paintComponent(Graphics g), javax.swing.Timer at 60 FPS,
 *    event-driven key handling, font metrics, glassmorphic HUD, overlays.
 * 2. Java Collections Framework:
 *    - Set<Integer> pressedKeys for smooth multi-key diagonal keyboard inputs.
 *    - List<Star> and List<Particle> for background parallax and explosive sparks.
 *    - Iterator safe removal during animation loops.
 * 3. Exception Handling:
 *    - Catches WeaponOverheatException and renders glowing HUD heat warning.
 *    - Catches GameOverException and transitions to game-over overlay with restart guide.
 * 4. Modular Packages: ui package communicating cleanly with service, model, exception.
 */

public class GamePanel extends JPanel implements ActionListener, KeyListener {
    public static final int PANEL_WIDTH = 850;
    public static final int PANEL_HEIGHT = 620;

    // Inner helper classes for visual effects
    private static class Star {
        double x, y, speed;
        int size;
        float alpha;
        Star(double x, double y, double speed, int size, float alpha) {
            this.x = x; this.y = y; this.speed = speed; this.size = size; this.alpha = alpha;
        }
    }

    private static class Particle {
        double x, y, vx, vy;
        Color color;
        int life, maxLife;
        Particle(double x, double y, double vx, double vy, Color color, int maxLife) {
            this.x = x; this.y = y; this.vx = vx; this.vy = vy; this.color = color;
            this.maxLife = maxLife; this.life = maxLife;
        }
        boolean update() {
            x += vx; y += vy; life--;
            return life > 0;
        }
    }

    private final GameEngine engine;
    private final LeaderboardService leaderboardService;
    private final Timer gameTimer;
    private final Random random = new Random();

    // Java Collections Framework in GUI:
    private final Set<Integer> pressedKeys = new HashSet<>();
    private final List<Star> starfield = new ArrayList<>();
    private final List<Particle> particles = new ArrayList<>();

    // UI state flags
    private boolean paused = false;
    private boolean overheatAlertActive = false;
    private int overheatAlertTimer = 0;
    private long animationFrame = 0;
    private boolean gameOverScoreRecorded = false;

    public GamePanel(final GameEngine engine, final LeaderboardService leaderboardService) {
        this.engine = engine;
        this.leaderboardService = leaderboardService;

        setPreferredSize(new Dimension(PANEL_WIDTH, PANEL_HEIGHT));
        setBackground(new Color(10, 14, 23)); // Deep Space Navy
        setFocusable(true);
        addKeyListener(this);

        initStarfield(90);

        // 60 FPS Swing Timer (~16.6ms per frame)
        this.gameTimer = new Timer(16, this);
        this.gameTimer.start();
    }

    private void initStarfield(final int count) {
        starfield.clear();
        for (int i = 0; i < count; i++) {
            double x = random.nextDouble() * PANEL_WIDTH;
            double y = random.nextDouble() * PANEL_HEIGHT;
            double speed = 0.5 + (random.nextDouble() * 2.5);
            int size = (speed > 2.0) ? 3 : (speed > 1.2 ? 2 : 1);
            float alpha = 0.3f + (random.nextFloat() * 0.7f);
            starfield.add(new Star(x, y, speed, size, alpha));
        }
    }

    @Override
    public void actionPerformed(final ActionEvent e) {
        animationFrame++;

        if (!paused && engine.isRunning()) {
            handleKeyboardInput();
            engine.update();
            updateParticles();
        } else if (!engine.isRunning() && !gameOverScoreRecorded) {
            // Automatically record & push final score to local storage and Supabase cloud on game over
            gameOverScoreRecorded = true;
            Player p = engine.getPlayer();
            if (p != null && leaderboardService != null) {
                System.out.printf("[GamePanel] >>> MISSION OVER: Auto-submitting score %d for %s (Wave %d) to Supabase Cloud...%n",
                        p.getScore(), p.getName(), engine.getCurrentWave());
                leaderboardService.recordScore(p.getName(), p.getScore(), engine.getCurrentWave());
                try {
                    leaderboardService.saveLeaderboard();
                } catch (Exception ignored) {}
            }
        }

        updateStarfield();

        // Handle visual overheat alert duration
        if (overheatAlertActive) {
            overheatAlertTimer--;
            if (overheatAlertTimer <= 0) {
                overheatAlertActive = false;
            }
        }

        repaint();
    }

    private void handleKeyboardInput() {
        double dx = 0.0;
        double dy = 0.0;

        if (pressedKeys.contains(KeyEvent.VK_W) || pressedKeys.contains(KeyEvent.VK_UP)) dy -= 1.0;
        if (pressedKeys.contains(KeyEvent.VK_S) || pressedKeys.contains(KeyEvent.VK_DOWN)) dy += 1.0;
        if (pressedKeys.contains(KeyEvent.VK_A) || pressedKeys.contains(KeyEvent.VK_LEFT)) dx -= 1.0;
        if (pressedKeys.contains(KeyEvent.VK_D) || pressedKeys.contains(KeyEvent.VK_RIGHT)) dx += 1.0;

        if (dx != 0.0 || dy != 0.0) {
            engine.movePlayer(dx, dy);
        }

        // Spacebar shooting with exception handling
        if (pressedKeys.contains(KeyEvent.VK_SPACE)) {
            // Rate limit firing to every 6 frames if held down
            if (animationFrame % 6 == 0) {
                try {
                    // Exception Handling: Calls checked method
                    Bullet bullet = engine.fireBulletChecked();
                    if (bullet != null) {
                        spawnThrusterSparks(engine.getPlayer().getX(), engine.getPlayer().getY(), Color.CYAN, 3);
                    }
                } catch (WeaponOverheatException woe) {
                    overheatAlertActive = true;
                    overheatAlertTimer = 30; // Flash for half a second
                } catch (GameOverException goe) {
                    // Handled gracefully in UI
                }
            }
        }
    }

    private void updateStarfield() {
        for (Star s : starfield) {
            s.x -= s.speed;
            if (s.x < 0) {
                s.x = PANEL_WIDTH;
                s.y = random.nextDouble() * PANEL_HEIGHT;
            }
        }
    }

    private void updateParticles() {
        // Java Collections: Iterator safe removal
        Iterator<Particle> it = particles.iterator();
        while (it.hasNext()) {
            Particle p = it.next();
            if (!p.update()) {
                it.remove();
            }
        }
    }

    public void spawnExplosion(final double x, final double y, final Color color, final int count) {
        for (int i = 0; i < count; i++) {
            double angle = random.nextDouble() * Math.PI * 2;
            double speed = 1.0 + (random.nextDouble() * 4.0);
            double vx = Math.cos(angle) * speed;
            double vy = Math.sin(angle) * speed;
            particles.add(new Particle(x, y, vx, vy, color, 15 + random.nextInt(15)));
        }
    }

    private void spawnThrusterSparks(final double x, final double y, final Color color, final int count) {
        for (int i = 0; i < count; i++) {
            double vx = -(2.0 + random.nextDouble() * 3.0);
            double vy = (random.nextDouble() - 0.5) * 2.0;
            particles.add(new Particle(x - 15, y, vx, vy, color, 8 + random.nextInt(6)));
        }
    }

    @Override
    protected void paintComponent(final Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g.create();

        // Enable crisp high quality antialiasing
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        // 1. Draw Starfield
        drawStarfield(g2);

        // 2. Draw Particles
        drawParticles(g2);

        // 3. Draw Laser Bullets
        drawBullets(g2);

        // 4. Draw Enemies (Polymorphic rendering)
        drawEnemies(g2);

        // 5. Draw Player Vessel
        drawPlayer(g2);

        // 6. Draw Top Glassmorphism HUD Bar
        drawHUD(g2);

        // 7. Draw Bottom Telemetry Bar
        drawBottomTelemetry(g2);

        // 8. Draw Overlays (Paused, Game Over)
        if (paused) {
            drawPauseOverlay(g2);
        } else if (!engine.isRunning()) {
            drawGameOverOverlay(g2);
        }

        g2.dispose();
    }

    private void drawStarfield(final Graphics2D g2) {
        for (Star s : starfield) {
            g2.setColor(new Color(1.0f, 1.0f, 1.0f, s.alpha));
            g2.fillRect((int) s.x, (int) s.y, s.size, s.size);
        }
    }

    private void drawParticles(final Graphics2D g2) {
        for (Particle p : particles) {
            float alpha = (float) p.life / p.maxLife;
            g2.setColor(new Color(p.color.getRed() / 255f, p.color.getGreen() / 255f, p.color.getBlue() / 255f, alpha));
            g2.fillRect((int) p.x, (int) p.y, 2, 2);
        }
    }

    private void drawBullets(final Graphics2D g2) {
        for (Bullet b : engine.getBullets()) {
            if (!b.isActive()) continue;
            int bx = (int) b.getX();
            int by = (int) b.getY();

            if (b.isPlayerBullet()) {
                // Neon Cyan Plasma Bolt
                g2.setColor(new Color(0, 255, 255, 120));
                g2.fillRect(bx - 12, by - 3, 14, 6);
                g2.setColor(new Color(220, 255, 255));
                g2.fillRect(bx - 8, by - 1, 10, 2);
            } else {
                // Crimson Enemy Bolt
                g2.setColor(new Color(255, 50, 50, 160));
                g2.fillOval(bx - 4, by - 4, 8, 8);
                g2.setColor(Color.WHITE);
                g2.fillOval(bx - 2, by - 2, 4, 4);
            }
        }
    }

    private void drawPlayer(final Graphics2D g2) {
        Player player = engine.getPlayer();
        if (!player.isAlive()) return;

        int px = (int) player.getX();
        int py = (int) player.getY();

        // Pulsating Engine Thruster Plume
        int thrusterFlicker = (int) (Math.sin(animationFrame * 0.8) * 4);
        int[] thrusterX = { px - 18, px - 32 - thrusterFlicker, px - 18 };
        int[] thrusterY = { py - 4, py, py + 4 };
        g2.setColor(new Color(255, 120, 0, 200));
        g2.fillPolygon(thrusterX, thrusterY, 3);
        int[] innerThrusterX = { px - 18, px - 24 - (thrusterFlicker / 2), px - 18 };
        int[] innerThrusterY = { py - 2, py, py + 2 };
        g2.setColor(new Color(0, 255, 255, 230));
        g2.fillPolygon(innerThrusterX, innerThrusterY, 3);

        // Fuselage Body (StarViper sleek aerodynamic shape)
        int[] shipX = { px + 22, px - 16, px - 8, px - 16 };
        int[] shipY = { py, py - 14, py, py + 14 };
        GradientPaint bodyGradient = new GradientPaint(px - 16, py, new Color(40, 70, 120),
                px + 22, py, new Color(0, 210, 255));
        g2.setPaint(bodyGradient);
        g2.fillPolygon(shipX, shipY, 4);

        g2.setColor(new Color(180, 240, 255));
        g2.setStroke(new BasicStroke(1.5f));
        g2.drawPolygon(shipX, shipY, 4);

        // Cockpit Canopy Glass
        g2.setColor(new Color(255, 255, 255, 220));
        g2.fillOval(px - 2, py - 3, 10, 6);

        // Deflector Shield Bubble (visible when shield > 0)
        if (player.getShield() > 0) {
            float shieldAlpha = Math.min(0.6f, (player.getShield() / 50.0f) * 0.5f + 0.1f);
            g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, shieldAlpha));
            g2.setColor(new Color(0, 255, 255));
            g2.drawOval(px - 24, py - 20, 52, 40);
            g2.setColor(new Color(0, 200, 255, 40));
            g2.fillOval(px - 24, py - 20, 52, 40);
            g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
        }
    }

    private void drawEnemies(final Graphics2D g2) {
        for (Enemy e : engine.getEnemies()) {
            if (!e.isActive()) continue;
            int ex = (int) e.getX();
            int ey = (int) e.getY();

            if (e instanceof Boss) {
                drawBoss(g2, (Boss) e, ex, ey);
            } else if (e instanceof Fighter) {
                drawFighter(g2, (Fighter) e, ex, ey);
            } else {
                drawDrone(g2, e, ex, ey);
            }
        }
    }

    private void drawDrone(final Graphics2D g2, final Enemy drone, final int x, final int y) {
        // Crimson Diamond Scout
        int[] dx = { x - 12, x, x + 12, x };
        int[] dy = { y, y - 10, y, y + 10 };
        g2.setColor(new Color(220, 40, 60));
        g2.fillPolygon(dx, dy, 4);
        g2.setColor(Color.WHITE);
        g2.setStroke(new BasicStroke(1.2f));
        g2.drawPolygon(dx, dy, 4);

        // Glowing red sensor eye
        g2.setColor(new Color(255, 200, 0));
        g2.fillOval(x - 3, y - 3, 6, 6);
    }

    private void drawFighter(final Graphics2D g2, final Fighter fighter, final int x, final int y) {
        // Violet/Purple Swept-Wing Interceptor
        int[] fx = { x - 16, x + 14, x + 6, x + 14, x - 16 };
        int[] fy = { y, y - 16, y, y + 16, y };
        g2.setColor(new Color(150, 40, 220));
        g2.fillPolygon(fx, fy, 5);
        g2.setColor(new Color(255, 120, 255));
        g2.setStroke(new BasicStroke(1.5f));
        g2.drawPolygon(fx, fy, 5);

        // Weapon pods
        g2.setColor(Color.YELLOW);
        g2.fillRect(x - 2, y - 14, 6, 3);
        g2.fillRect(x - 2, y + 11, 6, 3);
    }

    private void drawBoss(final Graphics2D g2, final Boss boss, final int x, final int y) {
        // Massive Obsidian Dreadnought
        boolean enrage = boss.isEnrageMode();

        if (enrage) {
            // Fiery enrage aura
            g2.setColor(new Color(255, 60, 0, 70));
            g2.fillOval(x - 55, y - 45, 110, 90);
        }

        // Heavy armor hull
        int[] bx = { x - 40, x + 25, x + 40, x + 25, x - 40, x - 20 };
        int[] by = { y - 30, y - 35, y, y + 35, y + 30, y };
        GradientPaint bossGradient = new GradientPaint(x - 40, y,
                enrage ? new Color(180, 20, 20) : new Color(50, 50, 60),
                x + 40, y,
                enrage ? new Color(255, 80, 0) : new Color(100, 110, 130));
        g2.setPaint(bossGradient);
        g2.fillPolygon(bx, by, 6);

        g2.setColor(enrage ? Color.RED : Color.CYAN);
        g2.setStroke(new BasicStroke(2.0f));
        g2.drawPolygon(bx, by, 6);

        // Reactor cores
        g2.setColor(enrage ? Color.YELLOW : Color.CYAN);
        g2.fillOval(x - 10, y - 8, 16, 16);
        g2.fillOval(x - 25, y - 20, 10, 10);
        g2.fillOval(x - 25, y + 10, 10, 10);

        // Boss Health Bar above ship
        int maxHp = boss.getMaxHealth();
        int curHp = boss.getHealth();
        double pct = (double) curHp / maxHp;
        g2.setColor(Color.DARK_GRAY);
        g2.fillRect(x - 35, y - 45, 70, 6);
        g2.setColor(enrage ? Color.RED : Color.ORANGE);
        g2.fillRect(x - 35, y - 45, (int) (70 * pct), 6);
        g2.setColor(Color.WHITE);
        g2.drawRect(x - 35, y - 45, 70, 6);
    }

    private void drawHUD(final Graphics2D g2) {
        // Glassmorphism HUD banner at top
        g2.setColor(new Color(15, 23, 42, 220));
        g2.fillRect(0, 0, PANEL_WIDTH, 56);
        g2.setColor(new Color(56, 189, 248, 80));
        g2.drawLine(0, 56, PANEL_WIDTH, 56);

        Player p = engine.getPlayer();

        // 1. Callsign and Wave
        g2.setFont(new Font("Monospaced", Font.BOLD, 14));
        g2.setColor(new Color(56, 189, 248));
        g2.drawString("PILOT: " + p.getName(), 18, 24);
        g2.setColor(Color.WHITE);
        g2.drawString("WAVE: " + engine.getCurrentWave(), 18, 44);

        // 2. Score with Gold readout
        g2.setFont(new Font("Monospaced", Font.BOLD, 18));
        g2.setColor(new Color(250, 204, 21)); // Gold
        g2.drawString(String.format("SCORE: %06d", p.getScore()), 200, 36);

        // 3. Health Gauge
        drawStatBar(g2, "HP", p.getHealth(), p.getMaxHealth(), 380, 16, 110, 14,
                new Color(34, 197, 94), new Color(239, 68, 68));

        // 4. Shield Gauge
        drawStatBar(g2, "SHIELD", p.getShield(), 50, 380, 34, 110, 14,
                new Color(6, 182, 212), new Color(30, 64, 175));

        // 5. Weapon Heat Gauge (Demonstrating Exception State Visualization)
        double heat = p.getWeaponHeat();
        double maxHeat = Player.MAX_WEAPON_HEAT;
        Color heatColor = (heat > 80.0) ? new Color(239, 68, 68) :
                          (heat > 50.0 ? new Color(245, 158, 11) : new Color(56, 189, 248));
        drawStatBar(g2, "HEAT", (int) heat, (int) maxHeat, 540, 24, 100, 16,
                heatColor, new Color(15, 23, 42));

        // Overheat Flashing Alert
        if (overheatAlertActive || p.isOverheated()) {
            if ((animationFrame / 8) % 2 == 0) {
                g2.setFont(new Font("SansSerif", Font.BOLD, 12));
                g2.setColor(new Color(239, 68, 68));
                g2.drawString("! OVERHEAT LOCKOUT !", 540, 52);
            }
        }

        // 6. Lives icons
        g2.setColor(Color.WHITE);
        g2.setFont(new Font("SansSerif", Font.BOLD, 12));
        g2.drawString("LIVES:", 710, 26);
        for (int i = 0; i < p.getLives(); i++) {
            int lx = 760 + (i * 22);
            int ly = 22;
            int[] ix = { lx + 8, lx - 6, lx - 2, lx - 6 };
            int[] iy = { ly, ly - 6, ly, ly + 6 };
            g2.setColor(new Color(0, 210, 255));
            g2.fillPolygon(ix, iy, 4);
        }

        // Leaderboard Hint
        g2.setFont(new Font("Monospaced", Font.PLAIN, 10));
        g2.setColor(new Color(148, 163, 184));
        g2.drawString("[H] High Scores | [P] Pause | [R] Restart", 660, 48);
    }

    private void drawStatBar(final Graphics2D g2, final String label, final int value, final int max,
                             final int x, final int y, final int w, final int h,
                             final Color primary, final Color warning) {
        g2.setFont(new Font("Monospaced", Font.BOLD, 10));
        g2.setColor(new Color(203, 213, 225));
        g2.drawString(label, x - 38, y + h - 2);

        // Background
        g2.setColor(new Color(30, 41, 59));
        g2.fillRect(x, y, w, h);

        // Fill
        double pct = Math.max(0.0, Math.min(1.0, (double) value / max));
        Color barColor = (pct < 0.35 && !label.equals("HEAT")) ? warning : primary;
        g2.setColor(barColor);
        g2.fillRect(x, y, (int) (w * pct), h);

        // Outline
        g2.setColor(new Color(100, 116, 139));
        g2.drawRect(x, y, w, h);

        // Text
        g2.setColor(Color.WHITE);
        String txt = value + "/" + max;
        g2.drawString(txt, x + (w / 2) - 16, y + h - 3);
    }

    private void drawBottomTelemetry(final Graphics2D g2) {
        // Bottom rolling combat event log from Queue/Deque collection
        g2.setColor(new Color(15, 23, 42, 210));
        g2.fillRect(0, PANEL_HEIGHT - 26, PANEL_WIDTH, 26);
        g2.setColor(new Color(56, 189, 248, 60));
        g2.drawLine(0, PANEL_HEIGHT - 26, PANEL_WIDTH, PANEL_HEIGHT - 26);

        Deque<String> logs = engine.getCombatEventLog();
        String latestEvent = logs.isEmpty() ? "[STATUS] Radar sweep active. No hostiles locked." : logs.peekLast();

        g2.setFont(new Font("Monospaced", Font.PLAIN, 11));
        g2.setColor(new Color(148, 163, 184));
        g2.drawString("COMBAT TELEMETRY: ", 16, PANEL_HEIGHT - 9);

        g2.setColor(latestEvent.contains("DESTROY") || latestEvent.contains("HIT") ? new Color(74, 222, 128) :
                   (latestEvent.contains("COLLISION") || latestEvent.contains("WARN") ? new Color(248, 113, 113) :
                    new Color(226, 232, 240)));
        g2.drawString(latestEvent, 150, PANEL_HEIGHT - 9);
    }

    private void drawPauseOverlay(final Graphics2D g2) {
        g2.setColor(new Color(10, 14, 23, 180));
        g2.fillRect(0, 0, PANEL_WIDTH, PANEL_HEIGHT);

        g2.setFont(new Font("SansSerif", Font.BOLD, 36));
        g2.setColor(new Color(56, 189, 248));
        String msg = "MISSION PAUSED";
        FontMetrics fm = g2.getFontMetrics();
        g2.drawString(msg, (PANEL_WIDTH - fm.stringWidth(msg)) / 2, 270);

        g2.setFont(new Font("Monospaced", Font.PLAIN, 16));
        g2.setColor(Color.WHITE);
        String sub = "Press [P] to Resume Mission | [R] to Restart";
        fm = g2.getFontMetrics();
        g2.drawString(sub, (PANEL_WIDTH - fm.stringWidth(sub)) / 2, 315);
    }

    private void drawGameOverOverlay(final Graphics2D g2) {
        g2.setColor(new Color(20, 10, 15, 220));
        g2.fillRect(0, 0, PANEL_WIDTH, PANEL_HEIGHT);

        g2.setFont(new Font("SansSerif", Font.BOLD, 42));
        g2.setColor(new Color(239, 68, 68));
        String msg = "MISSION FAILED";
        FontMetrics fm = g2.getFontMetrics();
        g2.drawString(msg, (PANEL_WIDTH - fm.stringWidth(msg)) / 2, 240);

        g2.setFont(new Font("Monospaced", Font.BOLD, 20));
        g2.setColor(new Color(250, 204, 21));
        String scoreMsg = String.format("FINAL SCORE: %d  |  WAVE REACHED: %d",
                engine.getPlayer().getScore(), engine.getCurrentWave());
        fm = g2.getFontMetrics();
        g2.drawString(scoreMsg, (PANEL_WIDTH - fm.stringWidth(scoreMsg)) / 2, 290);

        g2.setFont(new Font("Monospaced", Font.PLAIN, 16));
        g2.setColor(Color.WHITE);
        String restartMsg = "Press [R] to Launch New Mission  |  [H] View Leaderboard";
        fm = g2.getFontMetrics();
        g2.drawString(restartMsg, (PANEL_WIDTH - fm.stringWidth(restartMsg)) / 2, 345);
    }

    public void restartGame() {
        Player oldPlayer = engine.getPlayer();
        String pilotName = oldPlayer.getName();
        int finalScore = oldPlayer.getScore();
        int finalWave = engine.getCurrentWave();

        // Score was already recorded via the gameOverScoreRecorded auto-trigger in actionPerformed().
        // Do NOT call recordScore() here again — that caused duplicate Supabase submissions.

        engine.getPlayer().setHealth(100);
        engine.getPlayer().setShield(50);
        engine.getPlayer().setLives(3);
        engine.getPlayer().setScore(0);
        engine.getPlayer().setWeaponHeat(0.0);
        engine.getPlayer().setActive(true);
        engine.getPlayer().setX(100.0);
        engine.getPlayer().setY(300.0);
        engine.getEnemies().clear();
        engine.getBullets().clear();
        engine.setCurrentWave(1);
        engine.setRunning(true);
        engine.spawnWave(4);
        paused = false;
        gameOverScoreRecorded = false;
        pressedKeys.clear();
        particles.clear();
        engine.logEvent("[REBOOT] Fighter re-launched into combat orbit.");
    }

    public void togglePause() {
        this.paused = !this.paused;
    }

    public boolean isPaused() {
        return paused;
    }

    public void stopTimer() {
        if (gameTimer != null && gameTimer.isRunning()) {
            gameTimer.stop();
        }
    }

    public Timer getGameTimer() {
        return gameTimer;
    }

    // KeyListener implementations
    @Override
    public void keyPressed(final KeyEvent e) {
        int code = e.getKeyCode();
        pressedKeys.add(code);

        if (code == KeyEvent.VK_P) {
            togglePause();
        } else if (code == KeyEvent.VK_R) {
            restartGame();
        }
    }

    @Override
    public void keyReleased(final KeyEvent e) {
        pressedKeys.remove(e.getKeyCode());
    }

    @Override
    public void keyTyped(final KeyEvent e) {}
}
