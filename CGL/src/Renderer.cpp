#include "Renderer.h"
#include <cstdlib>
#include <cmath>

Renderer::Renderer() {
}

Point2D Renderer::translatePoint(Point2D p, float tx, float ty) {
    Point2D res;
    res.x = p.x + tx;
    res.y = p.y + ty;
    return res;
}

Point2D Renderer::scalePoint(Point2D p, float sx, float sy) {
    Point2D res;
    res.x = p.x * sx;
    res.y = p.y * sy;
    return res;
}

Point2D Renderer::rotatePoint(Point2D p, float angleRad) {
    Point2D res;
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);
    res.x = p.x * cosA - p.y * sinA;
    res.y = p.x * sinA + p.y * cosA;
    return res;
}

Point2D Renderer::transformPoint(Point2D p, float sx, float sy, float angleRad, float tx, float ty) {
    Point2D scaled = scalePoint(p, sx, sy);
    Point2D rotated = rotatePoint(scaled, angleRad);
    Point2D translated = translatePoint(rotated, tx, ty);
    return translated;
}

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

void Renderer::updateStarfield(float width, float height, float delta) {
    for (size_t i = 0; i < stars.size(); ++i) {
        stars[i].x -= stars[i].speed * delta * 60.0f;
        if (stars[i].x < 0.0f) {
            stars[i].x = width;
            stars[i].y = static_cast<float>(rand() % static_cast<int>(height));
        }
    }
}

void Renderer::setup2DProjection(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(width), 0.0, static_cast<double>(height));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::renderBackground(float width, float height) {
    glBegin(GL_QUADS);
        glColor3f(0.02f, 0.03f, 0.08f);
        glVertex2f(0.0f, height);
        glVertex2f(width, height);
        glColor3f(0.01f, 0.01f, 0.03f);
        glVertex2f(width, 0.0f);
        glVertex2f(0.0f, 0.0f);
    glEnd();

    for (size_t i = 0; i < stars.size(); ++i) {
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
            glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
            glVertex2f(stars[i].x, stars[i].y);
        glEnd();
    }
}

void Renderer::renderPlayerShip(float x, float y, float size, bool isAlive, float angle, float scale) {
    if (!isAlive) return;

    Point2D t1 = transformPoint({ -size * 0.6f, +size * 0.15f }, scale, scale, angle, x, y);
    Point2D t2 = transformPoint({ -size * 0.6f, -size * 0.15f }, scale, scale, angle, x, y);
    Point2D t3 = transformPoint({ -size * 1.1f, 0.0f }, scale, scale, angle, x, y);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.5f, 0.1f);
        glVertex2f(t1.x, t1.y);
        glVertex2f(t2.x, t2.y);
        glVertex2f(t3.x, t3.y);
    glEnd();

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

void Renderer::renderEnemyShip(float x, float y, float size, float angle, float scale) {
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

void Renderer::renderLaserBolt(float x, float y, float length, float width, bool isPlayer, float angle) {
    float halfW = width * 0.5f;
    float halfL = length * 0.5f;

    Point2D p1 = transformPoint({ -halfL, -halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p2 = transformPoint({ +halfL, -halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p3 = transformPoint({ +halfL, +halfW }, 1.0f, 1.0f, angle, x, y);
    Point2D p4 = transformPoint({ -halfL, +halfW }, 1.0f, 1.0f, angle, x, y);

    glBegin(GL_QUADS);
        if (isPlayer) {
            glColor3f(0.3f, 0.85f, 1.0f);
        } else {
            glColor3f(1.0f, 0.3f, 0.3f);
        }
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
    glEnd();
}

void Renderer::drawBitmapString(float x, float y, void* font, const std::string& text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void Renderer::renderHUD(int health, int maxHealth, int shield, int score, int wave,
                         int activeBullets, int maxBullets, float screenW, float screenH) {
    glBegin(GL_QUADS);
        glColor4f(0.05f, 0.08f, 0.14f, 0.85f);
        glVertex2f(0.0f, screenH);
        glVertex2f(screenW, screenH);
        glVertex2f(screenW, screenH - 50.0f);
        glVertex2f(0.0f, screenH - 50.0f);
    glEnd();

    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glColor3f(0.2f, 0.5f, 0.8f);
        glVertex2f(0.0f, screenH - 50.0f);
        glVertex2f(screenW, screenH - 50.0f);
    glEnd();

    float barX = 20.0f;
    float barY = screenH - 38.0f;
    float barW = 120.0f;
    float barH = 14.0f;

    float hpRatio = (maxHealth > 0) ? (static_cast<float>(health) / static_cast<float>(maxHealth)) : 0.0f;
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glColor3f(0.4f, 0.6f, 0.8f);
        glVertex2f(barX, barY);
        glVertex2f(barX + barW, barY);
        glVertex2f(barX + barW, barY + barH);
        glVertex2f(barX, barY + barH);
    glEnd();

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

    drawBitmapString(15.0f, 12.0f, GLUT_BITMAP_HELVETICA_10,
                     "[W/A/S/D] Move  |  [SPACE] Fire  |  [R] Restart  |  [ESC] Exit",
                     0.5f, 0.6f, 0.75f);
}

void Renderer::renderGameOver(int finalScore, float screenW, float screenH) {
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(screenW, 0.0f);
        glVertex2f(screenW, screenH);
        glVertex2f(0.0f, screenH);
    glEnd();

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

    drawBitmapString(boxX + 90.0f, boxY + 120.0f, GLUT_BITMAP_HELVETICA_18,
                     "MISSION FAILED", 1.0f, 0.2f, 0.2f);

    drawBitmapString(boxX + 110.0f, boxY + 80.0f, GLUT_BITMAP_HELVETICA_18,
                     "FINAL SCORE: " + std::to_string(finalScore), 1.0f, 0.9f, 0.3f);

    drawBitmapString(boxX + 80.0f, boxY + 40.0f, GLUT_BITMAP_HELVETICA_12,
                     "PRESS [R] TO RESTART MISSION", 0.8f, 0.9f, 1.0f);
}
