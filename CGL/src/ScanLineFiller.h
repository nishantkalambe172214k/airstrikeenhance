#ifndef SCANLINE_FILLER_H
#define SCANLINE_FILLER_H

#include <vector>
#include <algorithm>
#include "Renderer.h"
#include "Bresenham.h"

// ============================================================================
// [CGL CO-3] SCAN-LINE POLYGON FILL ALGORITHM
// Course Outcome 3: Develop 2D and 3D objects using various algorithms.
// - Genuine Edge Table (ET) & Active Edge Table (AET) rasterization.
// - Uses inverse slope (dx/dy) for incremental x intersection updates.
// - Employs odd-even parity rule to fill discrete horizontal spans.
// ============================================================================

// [CGL CO-3] Discrete horizontal pixel span on scanline y [xStart..xEnd]
struct HorizontalSpan {
    int y;
    int xStart;
    int xEnd;

    bool operator==(const HorizontalSpan& other) const {
        return y == other.y && xStart == other.xStart && xEnd == other.xEnd;
    }
};

// [CGL CO-3] Edge descriptor for Edge Table (ET) and Active Edge Table (AET)
struct EdgeTableEntry {
    int yMax;          // Upper scanline limit (half-open [yMin, yMax) prevents double vertex count)
    float xCurrent;    // Current x intersection coordinate on active scanline y
    float invSlope;    // 1/m = dx/dy (incremental change in x per unit increase in y)
    int yMin;          // Starting integer scanline y

    bool operator<(const EdgeTableEntry& other) const {
        return xCurrent < other.xCurrent;
    }
};

class ScanLineFiller {
public:
    // ========================================================================
    // PURE ALGORITHMIC CALCULATIONS (Testable headlessly without OpenGL)
    // ========================================================================

    // Generates horizontal scanline spans [y, xStart, xEnd] using the Edge Table (ET)
    // and Active Edge Table (AET) with the odd-even parity rule.
    static std::vector<HorizontalSpan> calculateScanlineSpans(const std::vector<Point2D>& vertices);

    // Converts calculated spans into discrete pixel coordinates (IntPoint2D)
    static std::vector<IntPoint2D> calculateFilledPixels(const std::vector<Point2D>& vertices);

    // ========================================================================
    // OPENGL RASTER RENDERING PRIMITIVES
    // ========================================================================

    // Renders the filled polygon by drawing each horizontal scanline span computed by the algorithm
    static void renderFilledPolygon(const std::vector<Point2D>& vertices,
                                   float r, float g, float b, float a = 1.0f);

    // Renders the filled polygon and strokes the perimeter using Bresenham lines
    static void renderFilledPolygonWithOutline(const std::vector<Point2D>& vertices,
                                              float fillR, float fillG, float fillB, float fillA,
                                              float edgeR, float edgeG, float edgeB, float edgeA,
                                              float lineWidth = 1.5f);
};

#endif // SCANLINE_FILLER_H
