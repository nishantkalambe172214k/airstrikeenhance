#include "Renderer.h"
#include <cstdlib>
#include <cmath>

Renderer::Renderer() {
}

// ============================================================================
// 2D GEOMETRIC TRANSFORMATIONS (Affine Math)
// ============================================================================

// Translates a 2D point by displacement offsets (tx, ty)
Point2D Renderer::translatePoint(Point2D p, float tx, float ty) {
    Point2D res;
    res.x = p.x + tx;
    res.y = p.y + ty;
    return res;
}

// Scales a 2D point relative to the origin (0, 0) by factors (sx, sy)
Point2D Renderer::scalePoint(Point2D p, float sx, float sy) {
    Point2D res;
    res.x = p.x * sx;
    res.y = p.y * sy;
    return res;
}

// Rotates a 2D point around the origin (0, 0) by angleRad radians using standard 2D rotation matrix:
// [ x' ] = [ cos(θ)  -sin(θ) ] [ x ]
// [ y' ]   [ sin(θ)   cos(θ) ] [ y ]
Point2D Renderer::rotatePoint(Point2D p, float angleRad) {
    Point2D res;
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);
    res.x = p.x * cosA - p.y * sinA;
    res.y = p.x * sinA + p.y * cosA;
    return res;
}

// Applies compound affine transformation in standard TRS order: Scale -> Rotate -> Translate
// Evaluated from local model space into world space coordinates
Point2D Renderer::transformPoint(Point2D p, float sx, float sy, float angleRad, float tx, float ty) {
    Point2D scaled = scalePoint(p, sx, sy);
    Point2D rotated = rotatePoint(scaled, angleRad);
    Point2D translated = translatePoint(rotated, tx, ty);
    return translated;
}

// ============================================================================
// PARALLAX STARFIELD SIMULATION
// ============================================================================

// Initializes stars across 3 depth tiers to create a multi-layer parallax effect
// Tier 1 (distant): slow, tiny, dim; Tier 3 (foreground): fast, large, bright
void Renderer::initStarfield(int count, float width, float height) {
    stars.clear();
    for (int i = 0; i < count; ++i) {
        Star s;
        s.x = static_cast<float>(rand() % static_cast<int>(width));
        s.y = static_cast<float>(rand() % static_cast<int>(height));
        int tier = (i % 3) + 1;
        s.speed = static_cast<float>(tier);
        s.size = (tier == 3) ? 2.5f : (tier == 2 ? 1.8f : 1.0f);
        s.brightness = 0.4f + tier * 0.2f;
        stars.push_back(s);
    }
}

// Moves stars leftward based on speed and delta time; wraps stars around screen edge
void Renderer::updateStarfield(float width, float height, float delta) {
    for (size_t i = 0; i < stars.size(); ++i) {
        stars[i].x -= stars[i].speed * delta * 60.0f;
        // Screen wrap-around when star exits left boundary
        if (stars[i].x < 0.0f) {
            stars[i].x = width;
            stars[i].y = static_cast<float>(rand() % static_cast<int>(height));
        }
    }
}

// ============================================================================
// OPENGL VIEWPORT & PROJECTION SETUP
// ============================================================================

// Sets up a 2D orthographic projection matching window dimensions (pixel coordinates)
// Origin (0,0) is bottom-left, (width, height) is top-right. Enables alpha blending.
void Renderer::setup2DProjection(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(width), 0.0, static_cast<double>(height));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Standard alpha blending for transparency effects (HUD, overlays, lasers)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ============================================================================
// PRIMITIVE RENDERING ROUTINES
// ============================================================================

// Draws deep space vertical gradient backdrop and point-sprite parallax stars
void Renderer::renderBackground(float width, float height) {
    // Vertical color gradient (dark navy blue at top to near black at bottom)
    glBegin(GL_QUADS);
        glColor3f(0.02f, 0.03f, 0.08f);
        glVertex2f(0.0f, height);
        glVertex2f(width, height);
        glColor3f(0.01f, 0.01f, 0.03f);
        glVertex2f(width, 0.0f);
        glVertex2f(0.0f, 0.0f);
    glEnd();

    // Parallax star particles
    for (size_t i = 0; i < stars.size(); ++i) {
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
            glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
            glVertex2f(stars[i].x, stars[i].y);
        glEnd();
    }
}

// Renders player spaceship using multi-part geometric primitives:
// 1. Thruster exhaust flame (triangle)
// 2. Main swept wings (quadrilateral)
// 3. Central aerodynamic fuselage (gradient triangle)
// 4. Glass cockpit canopy (diamond polygon)
void Renderer::renderPlayerShip(float x, float y, float size, bool isAlive, float angle, float scale) {
    if (!isAlive) return;

    // 1. Thruster flame (rear orange/yellow triangle)
    Point2D t1 = transformPoint({ -size * 0.6f, +size * 0.15f }, scale, scale, angle, x, y);
    Point2D t2 = transformPoint({ -size * 0.6f, -size * 0.15f }, scale, scale, angle, x, y);
    Point2D t3 = transformPoint({ -size * 1.1f, 0.0f }, scale, scale, angle, x, y);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.5f, 0.1f);
        glVertex2f(t1.x, t1.y);
        glVertex2f(t2.x, t2.y);
        glVertex2f(t3.x, t3.y);
    glEnd();

    // 2. Wings (swept blue quad)
    Point2D w1 = transformPoint({ -size * 0.2f, +size * 0.2f }, scale, scale, angle, x, y);
    Point2D w2 = transformPoint({ -size * 0.8f, +size * 0.8f }, scale, scale, angle, x, y);
    Point2D w3 = transformPoint({ -size * 0.8f, -size * 0.8f }, scale, scale, angle, x, y);
    Point2D w4 = transformPoint({ -size * 0.2f, -size * 0.2f }, scale, scale, angle, x, y);

    glBegin(GL_QUADS);
        glColor3f(0.08f, 0.35f, 0.70f);
        glVertex2f(w1.x, w1.y);
        glVertex2f(w2.x, w2.y);
        glVertex2f(w3.x, w3.y);
        glVertex2f(w4.x, w4.y);
    glEnd();

    // 3. Fuselage / Main hull (forward-facing shaded triangle)
    Point2D f1 = transformPoint({ +size, 0.0f }, scale, scale, angle, x, y);
    Point2D f2 = transformPoint({ -size * 0.6f, +size * 0.35f }, scale, scale, angle, x, y);
    Point2D f3 = transformPoint({ -size * 0.6f, -size * 0.35f }, scale, scale, angle, x, y);

    glBegin(GL_TRIANGLES);
        glColor3f(0.2f, 0.65f, 0.98f);
        glVertex2f(f1.x, f1.y);
        glColor3f(0.1f, 0.45f, 0.8f);
        glVertex2f(f2.x, f2.y);
        glVertex2f(f3.x, f3.y);
    glEnd();

    // 4. Cockpit canopy (cyan diamond polygon)
    Point2D c1 = transformPoint({ +size * 0.4f, 0.0f }, scale, scale, angle, x, y);
    Point2D c2 = transformPoint({ 0.0f, +size * 0.15f }, scale, scale, angle, x, y);
    Point2D c3 = transformPoint({ -size * 0.2f, 0.0f }, scale, scale, angle, x, y);
    Point2D c4 = transformPoint({ 0.0f, -size * 0.15f }, scale, scale, angle, x, y);

    glBegin(GL_POLYGON);
        glColor3f(0.7f, 0.95f, 1.0f);
        glVertex2f(c1.x, c1.y);
        glVertex2f(c2.x, c2.y);
        glVertex2f(c3.x, c3.y);
        glVertex2f(c4.x, c4.y);
    glEnd();
}

// Renders enemy drone using aggressive crimson & amber geometric primitives:
// 1. Forward-swept dark red wings (quad)
// 2. Dagger-shaped main chassis (triangle)
// 3. Amber energy core cockpit (polygon)
void Renderer::renderEnemyShip(float x, float y, float size, float angle, float scale) {
    // 1. Wings (crimson quad)
    Point2D w1 = transformPoint({ 0.0f, +size * 0.2f }, scale, scale, angle, x, y);
    Point2D w2 = transformPoint({ +size * 0.7f, +size * 0.7f }, scale, scale, angle, x, y);
    Point2D w3 = transformPoint({ +size * 0.7f, -size * 0.7f }, scale, scale, angle, x, y);
    Point2D w4 = transformPoint({ 0.0f, -size * 0.2f }, scale, scale, angle, x, y);

    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.08f, 0.12f);
        glVertex2f(w1.x, w1.y);
        glVertex2f(w2.x, w2.y);
        glVertex2f(w3.x, w3.y);
        glVertex2f(w4.x, w4.y);
    glEnd();

    // 2. Main chassis (red shaded dagger triangle pointing left)
    Point2D b1 = transformPoint({ -size * 0.8f, 0.0f }, scale, scale, angle, x, y);
    Point2D b2 = transformPoint({ +size * 0.6f, +size * 0.4f }, scale, scale, angle, x, y);
    Point2D b3 = transformPoint({ +size * 0.6f, -size * 0.4f }, scale, scale, angle, x, y);

    glBegin(GL_TRIANGLES);
        glColor3f(0.9f, 0.2f, 0.2f);
        glVertex2f(b1.x, b1.y);
        glColor3f(0.6f, 0.1f, 0.15f);
        glVertex2f(b2.x, b2.y);
        glVertex2f(b3.x, b3.y);
    glEnd();

    // 3. Energy core canopy (amber polygon)
    Point2D c1 = transformPoint({ -size * 0.2f, 0.0f }, scale, scale, angle, x, y);
    Point2D c2 = transformPoint({ 0.0f, +size * 0.12f }, scale, scale, angle, x, y);
    Point2D c3 = transformPoint({ +size * 0.2f, 0.0f }, scale, scale, angle, x, y);
    Point2D c4 = transformPoint({ 0.0f, -size * 0.12f }, scale, scale, angle, x, y);

    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.8f, 0.2f);
        glVertex2f(c1.x, c1.y);
        glVertex2f(c2.x, c2.y);
        glVertex2f(c3.x, c3.y);
        glVertex2f(c4.x, c4.y);
    glEnd();
}

// Renders an oriented laser projectile quad (cyan for player, red for enemy)
void Renderer::renderLaserBolt(float x, float y, float length, float width, bool isPlayer, float angle) {
    float halfW = width * 0.5f;
    float halfL = length * 0.5f;

    Point2D p1 = transformPoint({ -halfL, -halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p2 = transformPoint({ +halfL, -halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p3 = transformPoint({ +halfL, +halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p4 = transformPoint({ -halfL, +halfW }, 1.0f, 1.0f, angle, x, y);

    glBegin(GL_QUADS);
        if (isPlayer) {
            glColor3f(0.3f, 0.85f, 1.0f); // Cyan laser beam
        } else {
            glColor3f(1.0f, 0.3f, 0.3f);  // Red enemy beam
        }
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
    glEnd();
}

// ============================================================================
// UI & HUD RENDERING
// ============================================================================

// Utility to render bitmap text strings at screen coordinates (x, y) with specified RGB color
void Renderer::drawBitmapString(float x, float y, void* font, const std::string& text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// Renders Heads-Up Display (HUD) banner, dynamic health bar, shield, score, wave, and ammo
void Renderer::renderHUD(int health, int maxHealth, int shield, int score, int wave,
                         int activeBullets, int maxBullets, float screenW, float screenH) {
    // 1. Semi-transparent top HUD banner bar
    glBegin(GL_QUADS);
        glColor4f(0.05f, 0.08f, 0.14f, 0.85f);
        glVertex2f(0.0f, screenH);
        glVertex2f(screenW, screenH);
        glVertex2f(screenW, screenH - 50.0f);
        glVertex2f(0.0f, screenH - 50.0f);
    glEnd();

    // Accent separation border line
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glColor3f(0.2f, 0.5f, 0.8f);
        glVertex2f(0.0f, screenH - 50.0f);
        glVertex2f(screenW, screenH - 50.0f);
    glEnd();

    // 2. Health Bar (Outline + Dynamic Fill with adaptive color thresholds)
    float barX = 20.0f;
    float barY = screenH - 38.0f;
    float barW = 120.0f;
    float barH = 14.0f;

    float hpRatio = (maxHealth > 0) ? (static_cast<float>(health) / static_cast<float>(maxHealth)) : 0.0f;
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    // Health bar outer frame
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glColor3f(0.4f, 0.6f, 0.8f);
        glVertex2f(barX, barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH);
        glVertex2f(barX, barY + barH);
    glEnd();

    // Health bar inner fill: Green (>50%), Yellow (25-50%), Red (<25%)
    glBegin(GL_QUADS);
        if (hpRatio > 0.5f) {
            glColor3f(0.2f, 0.85f, 0.3f);
        } else if (hpRatio > 0.25f) {
            glColor3f(0.9f, 0.8f, 0.2f);
        } else {
            glColor3f(0.9f, 0.2f, 0.2f);
        }
        glVertex2f(barX, barY);
        glVertex2f(barX + (barW * hpRatio), barY);
        glVertex2f(barX + (barW * hpRatio), barY + barH);
        glVertex2f(barX, barY + barH);
    glEnd();

    // 3. HUD Telemetry Texts
    drawBitmapString(barX, screenH - 18.0f, GLUT_BITMAP_HELVETICA_12,
                     "HP: " + std::to_string(health), 0.9f, 0.9f, 1.0f);

    drawBitmapString(barX + barW + 15.0f, screenH - 25.0f, GLUT_BITMAP_HELVETICA_12,
                     "SHIELD: " + std::to_string(shield), 0.3f, 0.75f, 1.0f);

    drawBitmapString(260.0f, screenH - 25.0f, GLUT_BITMAP_HELVETICA_18,
                     "SCORE: " + std::to_string(score), 1.0f, 0.9f, 0.2f);

    drawBitmapString(400.0f, screenH - 25.0f, GLUT_BITMAP_HELVETICA_18,
                     "WAVE: " + std::to_string(wave), 0.3f, 0.85f, 1.0f);

    drawBitmapString(560.0f, screenH - 25.0f, GLUT_BITMAP_HELVETICA_12,
                     "BULLETS: " + std::to_string(activeBullets) + "/" + std::to_string(maxBullets),
                     0.7f, 0.7f, 0.85f);

    // Controls legend at bottom of screen
    drawBitmapString(15.0f, 12.0f, GLUT_BITMAP_HELVETICA_10,
                     "[W/A/S/D] Move  |  [SPACE] Fire  |  [R] Restart  |  [ESC] Exit",
                     0.5f, 0.6f, 0.75f);
}

// Renders modal Game Over screen overlay with score summary and restart prompt
void Renderer::renderGameOver(int finalScore, float screenW, float screenH) {
    // Dimmed translucent backdrop overlay
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(screenW, 0.0f);
        glVertex2f(screenW, screenH);
        glVertex2f(0.0f, screenH);
    glEnd();

    // Centered modal alert dialog box
    float boxW = 400.0f;
    float boxH = 180.0f;
    float boxX = (screenW - boxW) * 0.5f;
    float boxY = (screenH - boxH) * 0.5f;

    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glColor3f(0.9f, 0.2f, 0.2f);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Game over text announcements
    drawBitmapString(boxX + 90.0f, boxY + 120.0f, GLUT_BITMAP_HELVETICA_18,
                     "MISSION FAILED", 1.0f, 0.2f, 0.2f);

    drawBitmapString(boxX + 110.0f, boxY + 80.0f, GLUT_BITMAP_HELVETICA_18,
                     "FINAL SCORE: " + std::to_string(finalScore), 1.0f, 0.9f, 0.3f);

    drawBitmapString(boxX + 80.0f, boxY + 40.0f, GLUT_BITMAP_HELVETICA_12,
                     "PRESS [R] TO RESTART MISSION", 0.8f, 0.9f, 1.0f);
}
