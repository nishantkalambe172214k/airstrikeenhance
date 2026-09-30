#ifndef BRESENHAM_H
#define BRESENHAM_H

#include <vector>

struct IntPoint2D {
    int x;
    int y;

    bool operator==(const IntPoint2D& other) const {
        return x == other.x && y == other.y;
    }
};

class Bresenham {
public:
    // ========================================================================
    // PURE ALGORITHMIC CALCULATIONS (Testable without OpenGL context)
    // ========================================================================

    // Generalized Mid-point Bresenham Line Algorithm for all 8 octants
    static std::vector<IntPoint2D> calculateLine(int x0, int y0, int x1, int y1);

    // Mid-point Bresenham Circle Algorithm using 8-way symmetry
    static std::vector<IntPoint2D> calculateCircle(int xc, int yc, int radius);

    // Circular Arc calculation between startAngleRad and endAngleRad
    static std::vector<IntPoint2D> calculateArc(int xc, int yc, int radius, float startAngleRad, float endAngleRad);

    // ========================================================================
    // OPENGL RASTER RENDERING PRIMITIVES
    // ========================================================================

    // Draws a line using Bresenham algorithm points via GL_POINTS
    static void renderLine(int x0, int y0, int x1, int y1,
                           float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                           float pointSize = 1.5f);

    // Draws a circle using Bresenham 8-way symmetric algorithm points
    static void renderCircle(int xc, int yc, int radius,
                             float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                             float pointSize = 1.5f);

    // Draws a dashed circular ring (e.g. for radar range rings or targeting lock reticle)
    static void renderCircleDashed(int xc, int yc, int radius, int segments = 12,
                                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                                  float pointSize = 1.5f);

    // Renders a complete targeting crosshair reticle (Circle + 4 Crosshair Lines)
    static void renderReticle(int cx, int yc, int radius, int spikeLength = 8,
                              float r = 1.0f, float g = 0.2f, float b = 0.2f, float a = 1.0f);
};

#endif // BRESENHAM_H
