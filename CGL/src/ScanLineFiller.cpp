#include "ScanLineFiller.h"
#include <cmath>
#include <map>

// ============================================================================
// PURE ALGORITHMIC CALCULATIONS (Testable headlessly without OpenGL)
// ============================================================================

// Generates horizontal scanline spans [y, xStart, xEnd] using the Edge Table (ET)
// and Active Edge Table (AET) with the odd-even parity rule.
std::vector<HorizontalSpan> ScanLineFiller::calculateScanlineSpans(const std::vector<Point2D>& vertices) {
    std::vector<HorizontalSpan> spans;
    int n = static_cast<int>(vertices.size());
    if (n < 3) return spans;

    // Determine global y bounds across all polygon vertices
    float minYFloat = vertices[0].y;
    float maxYFloat = vertices[0].y;
    for (int i = 1; i < n; ++i) {
        if (vertices[i].y < minYFloat) minYFloat = vertices[i].y;
        if (vertices[i].y > maxYFloat) maxYFloat = vertices[i].y;
    }

    int globalMinY = static_cast<int>(std::floor(minYFloat));
    int globalMaxY = static_cast<int>(std::ceil(maxYFloat));

    // ========================================================================
    // [CGL CO-3] STEP 1: Build Edge Table (ET) bucketed by integer scanline yMin.
    // Completely horizontal edges (dy == 0) are excluded since scanlines cover them.
    // Half-open interval [yMin, yMax) ensures shared vertices are counted once.
    // ========================================================================
    std::map<int, std::vector<EdgeTableEntry>> edgeTable;

    for (int i = 0; i < n; ++i) {
        Point2D p1 = vertices[i];
        Point2D p2 = vertices[(i + 1) % n];

        // Skip completely horizontal edges to avoid division by zero
        if (std::abs(p2.y - p1.y) < 1e-5f) {
            continue;
        }

        Point2D lower = (p1.y < p2.y) ? p1 : p2;
        Point2D upper = (p1.y < p2.y) ? p2 : p1;

        // Use half-open scanline intervals [yStart, yEnd) for vertex parity conservation
        int yStart = static_cast<int>(std::ceil(lower.y));
        int yEnd = static_cast<int>(std::ceil(upper.y));

        if (yStart >= yEnd) {
            continue; // Degenerate sub-pixel vertical height
        }

        // [CGL CO-3] Inverse slope: 1/m = dx / dy
        float invM = (upper.x - lower.x) / (upper.y - lower.y);
        // Calculate sub-pixel x intersection at the first integer scanline yStart
        float initialX = lower.x + (static_cast<float>(yStart) - lower.y) * invM;

        EdgeTableEntry entry;
        entry.yMin = yStart;
        entry.yMax = yEnd;
        entry.xCurrent = initialX;
        entry.invSlope = invM;

        edgeTable[yStart].push_back(entry);
    }

    // Sort edges in each ET bucket by initial xCurrent
    for (auto& pair : edgeTable) {
        std::sort(pair.second.begin(), pair.second.end());
    }

    // ========================================================================
    // [CGL CO-3] STEP 2: Active Edge Table (AET) Scanline Evaluation
    // Sweep y from globalMinY to globalMaxY, pairing intersections.
    // ========================================================================
    std::vector<EdgeTableEntry> aet;

    for (int y = globalMinY; y <= globalMaxY; ++y) {
        // [CGL CO-3] Step 2A: Move new active edges from ET[y] into AET
        if (edgeTable.find(y) != edgeTable.end()) {
            const auto& newEdges = edgeTable[y];
            aet.insert(aet.end(), newEdges.begin(), newEdges.end());
        }

        // [CGL CO-3] Step 2B: Remove expired edges from AET where y >= yMax
        aet.erase(std::remove_if(aet.begin(), aet.end(),
            [y](const EdgeTableEntry& e) { return y >= e.yMax; }),
            aet.end());

        if (aet.empty()) {
            continue;
        }

        // [CGL CO-3] Step 2C: Sort AET by current x intersection in ascending order
        std::sort(aet.begin(), aet.end());

        // [CGL CO-3] Step 2D: Odd-Even Parity Rule — fill spans between (2k, 2k+1) pairs
        for (size_t k = 0; k + 1 < aet.size(); k += 2) {
            int xStart = static_cast<int>(std::ceil(aet[k].xCurrent));
            int xEnd = static_cast<int>(std::floor(aet[k + 1].xCurrent));

            // Prevent precision underflow for narrow vertical strips
            if (xStart > xEnd && (aet[k + 1].xCurrent - aet[k].xCurrent) >= 0.0f) {
                xEnd = xStart;
            }

            if (xStart <= xEnd) {
                spans.push_back({ y, xStart, xEnd });
            }
        }

        // [CGL CO-3] Step 2E: Incremental update: xCurrent += invSlope (dx/dy)
        for (auto& edge : aet) {
            edge.xCurrent += edge.invSlope;
        }
    }

    return spans;
}

// Converts calculated spans into discrete pixel coordinates (IntPoint2D)
std::vector<IntPoint2D> ScanLineFiller::calculateFilledPixels(const std::vector<Point2D>& vertices) {
    std::vector<IntPoint2D> pixels;
    std::vector<HorizontalSpan> spans = calculateScanlineSpans(vertices);
    for (const auto& span : spans) {
        for (int x = span.xStart; x <= span.xEnd; ++x) {
            pixels.push_back({ x, span.y });
        }
    }
    return pixels;
}

// ============================================================================
// OPENGL RASTER RENDERING PRIMITIVES
// ============================================================================

// Renders the filled polygon by drawing each horizontal scanline span computed by the algorithm
void ScanLineFiller::renderFilledPolygon(const std::vector<Point2D>& vertices,
                                         float r, float g, float b, float a) {
    std::vector<HorizontalSpan> spans = calculateScanlineSpans(vertices);
    if (spans.empty()) return;

    glColor4f(r, g, b, a);
    glBegin(GL_LINES);
    for (const auto& span : spans) {
        glVertex2f(static_cast<float>(span.xStart), static_cast<float>(span.y));
        glVertex2f(static_cast<float>(span.xEnd + 1), static_cast<float>(span.y));
    }
    glEnd();
}

// Renders the filled polygon and strokes the perimeter using Bresenham lines
void ScanLineFiller::renderFilledPolygonWithOutline(const std::vector<Point2D>& vertices,
                                                   float fillR, float fillG, float fillB, float fillA,
                                                   float edgeR, float edgeG, float edgeB, float edgeA,
                                                   float lineWidth) {
    renderFilledPolygon(vertices, fillR, fillG, fillB, fillA);

    int n = static_cast<int>(vertices.size());
    if (n < 2) return;

    for (int i = 0; i < n; ++i) {
        Point2D p1 = vertices[i];
        Point2D p2 = vertices[(i + 1) % n];
        Bresenham::renderLine(static_cast<int>(p1.x), static_cast<int>(p1.y),
                              static_cast<int>(p2.x), static_cast<int>(p2.y),
                              edgeR, edgeG, edgeB, edgeA, lineWidth);
    }
}
