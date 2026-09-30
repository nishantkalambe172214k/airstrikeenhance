#include "Bresenham.h"
#include <cmath>
#include <algorithm>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

// ============================================================================
// PURE ALGORITHMIC IMPLEMENTATIONS
// ============================================================================

// Generalized Mid-point Bresenham Line Algorithm for all slopes and octants
std::vector<IntPoint2D> Bresenham::calculateLine(int x0, int y0, int x1, int y1) {
    std::vector<IntPoint2D> points;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int currX = x0;
    int currY = y0;

    while (true) {
        points.push_back({ currX, currY });
        if (currX == x1 && currY == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            currX += sx;
        }
        if (e2 < dx) {
            err += dx;
            currY += sy;
        }
    }

    return points;
}

// Mid-point Bresenham Circle Algorithm using 8-way symmetry
std::vector<IntPoint2D> Bresenham::calculateCircle(int xc, int yc, int radius) {
    std::vector<IntPoint2D> points;
    if (radius <= 0) {
        points.push_back({ xc, yc });
        return points;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    // Helper lambda to insert 8 symmetric octant coordinates
    auto add8Points = [&](int px, int py) {
        points.push_back({ xc + px, yc + py });
        points.push_back({ xc - px, yc + py });
        points.push_back({ xc + px, yc - py });
        points.push_back({ xc - px, yc - py });
        points.push_back({ xc + py, yc + px });
        points.push_back({ xc - py, yc + px });
        points.push_back({ xc + py, yc - px });
        points.push_back({ xc - py, yc - px });
    };

    while (x <= y) {
        add8Points(x, y);
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }

    return points;
}

std::vector<IntPoint2D> Bresenham::calculateArc(int xc, int yc, int radius, float startAngleRad, float endAngleRad) {
    std::vector<IntPoint2D> allPoints = calculateCircle(xc, yc, radius);
    std::vector<IntPoint2D> arcPoints;

    const float TWO_PI = 6.2831853f;
    // Normalize angles to [0, 2PI)
    while (startAngleRad < 0.0f) startAngleRad += TWO_PI;
    while (startAngleRad >= TWO_PI) startAngleRad -= TWO_PI;
    while (endAngleRad < 0.0f) endAngleRad += TWO_PI;
    while (endAngleRad >= TWO_PI) endAngleRad -= TWO_PI;

    for (const auto& pt : allPoints) {
        float angle = std::atan2(static_cast<float>(pt.y - yc), static_cast<float>(pt.x - xc));
        if (angle < 0.0f) angle += TWO_PI;

        bool inRange = false;
        if (startAngleRad <= endAngleRad) {
            inRange = (angle >= startAngleRad && angle <= endAngleRad);
        } else {
            inRange = (angle >= startAngleRad || angle <= endAngleRad);
        }

        if (inRange) {
            arcPoints.push_back(pt);
        }
    }
    return arcPoints;
}

// ============================================================================
// OPENGL RASTER RENDERING PRIMITIVES
// ============================================================================

void Bresenham::renderLine(int x0, int y0, int x1, int y1,
                           float r, float g, float b, float a, float pointSize) {
    std::vector<IntPoint2D> pts = calculateLine(x0, y0, x1, y1);

    glPointSize(pointSize);
    glColor4f(r, g, b, a);
    glBegin(GL_POINTS);
    for (const auto& pt : pts) {
        glVertex2i(pt.x, pt.y);
    }
    glEnd();
}

void Bresenham::renderCircle(int xc, int yc, int radius,
                             float r, float g, float b, float a, float pointSize) {
    std::vector<IntPoint2D> pts = calculateCircle(xc, yc, radius);

    glPointSize(pointSize);
    glColor4f(r, g, b, a);
    glBegin(GL_POINTS);
    for (const auto& pt : pts) {
        glVertex2i(pt.x, pt.y);
    }
    glEnd();
}

void Bresenham::renderCircleDashed(int xc, int yc, int radius, int segments,
                                  float r, float g, float b, float a, float pointSize) {
    std::vector<IntPoint2D> pts = calculateCircle(xc, yc, radius);
    if (pts.empty()) return;

    glPointSize(pointSize);
    glColor4f(r, g, b, a);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < pts.size(); ++i) {
        // Dash pattern: draw only in alternating intervals
        if ((i / segments) % 2 == 0) {
            glVertex2i(pts[i].x, pts[i].y);
        }
    }
    glEnd();
}

void Bresenham::renderReticle(int cx, int cy, int radius, int spikeLength,
                              float r, float g, float b, float a) {
    // 1. Center circular ring via Bresenham Circle
    renderCircle(cx, cy, radius, r, g, b, a, 1.8f);

    // 2. Crosshair spikes via Bresenham Line
    // North spike
    renderLine(cx, cy + radius, cx, cy + radius + spikeLength, r, g, b, a, 2.0f);
    // South spike
    renderLine(cx, cy - radius, cx, cy - radius - spikeLength, r, g, b, a, 2.0f);
    // East spike
    renderLine(cx + radius, cy, cx + radius + spikeLength, cy, r, g, b, a, 2.0f);
    // West spike
    renderLine(cx - radius, cy, cx - radius - spikeLength, cy, r, g, b, a, 2.0f);
}
