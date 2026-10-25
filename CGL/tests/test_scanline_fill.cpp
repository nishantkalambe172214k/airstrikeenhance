#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <set>
#include "../src/ScanLineFiller.h"

void testTriangleScanlineFill() {
    std::cout << "[TEST 1] Scan-Line Fill: Isosceles Triangle with Horizontal Base...\n";

    // Triangle: base from (10, 10) to (30, 10), apex at (20, 25)
    std::vector<Point2D> tri = {
        { 10.0f, 10.0f },
        { 30.0f, 10.0f },
        { 20.0f, 25.0f }
    };

    auto spans = ScanLineFiller::calculateScanlineSpans(tri);
    assert(!spans.empty());

    // Height = 25 - 10 = 15 scanlines (y = 10 to 24)
    assert(spans.size() == 15);
    assert(spans.front().y == 10);
    assert(spans.back().y == 24);

    // Verify horizontal base span at y=10
    assert(spans[0].xStart == 10);
    assert(spans[0].xEnd == 30);

    // Verify symmetric contraction toward apex (at y=24, apex width is ~1-2 pixels around x=20)
    for (const auto& s : spans) {
        assert(s.xStart <= s.xEnd);
        assert(s.xStart >= 10 && s.xEnd <= 30);
    }
    assert(std::abs(spans.back().xStart - 20) <= 1);
    assert(std::abs(spans.back().xEnd - 20) <= 1);

    std::cout << " -> Triangle successfully rasterized into " << spans.size() << " symmetric scanline spans.\n";
}

void testConvexRectangleScanlineFill() {
    std::cout << "\n[TEST 2] Scan-Line Fill: Axis-Aligned Convex Rectangle...\n";

    // Rectangle: (10, 20) to (40, 50)
    std::vector<Point2D> rect = {
        { 10.0f, 20.0f },
        { 40.0f, 20.0f },
        { 40.0f, 50.0f },
        { 10.0f, 50.0f }
    };

    auto spans = ScanLineFiller::calculateScanlineSpans(rect);
    assert(!spans.empty());

    // 50 - 20 = 30 scanlines
    assert(spans.size() == 30);
    for (size_t i = 0; i < spans.size(); ++i) {
        assert(spans[i].y == static_cast<int>(20 + i));
        assert(spans[i].xStart == 10);
        assert(spans[i].xEnd == 40);
    }

    auto pixels = ScanLineFiller::calculateFilledPixels(rect);
    // 30 scanlines * (40 - 10 + 1 = 31 pixels) = 930 pixels
    assert(pixels.size() == 30 * 31);

    std::cout << " -> Convex rectangle filled across 30 scanlines with exact bounds [10, 40] (930 pixels).\n";
}

void testConcavePolygonOddEvenParity() {
    std::cout << "\n[TEST 3] Scan-Line Fill: Concave V-Notch Polygon (Odd-Even Parity Rule)...\n";

    // U-shaped concave polygon with a center recess
    std::vector<Point2D> uShape = {
        { 0.0f, 0.0f },
        { 30.0f, 0.0f },
        { 30.0f, 30.0f },
        { 20.0f, 30.0f },
        { 20.0f, 10.0f },
        { 10.0f, 10.0f },
        { 10.0f, 30.0f },
        { 0.0f, 30.0f }
    };

    auto spans = ScanLineFiller::calculateScanlineSpans(uShape);
    assert(!spans.empty());

    // Check solid base (y = 0 to 9): single span from 0 to 30
    for (const auto& s : spans) {
        if (s.y >= 0 && s.y < 10) {
            assert(s.xStart == 0 && s.xEnd == 30);
        }
    }

    // Check two branches in the notched region (y = 10 to 29): should have two distinct spans per scanline
    int twoSpanCount = 0;
    for (int y = 10; y < 30; ++y) {
        std::vector<HorizontalSpan> ySpans;
        for (const auto& s : spans) {
            if (s.y == y) ySpans.push_back(s);
        }
        if (ySpans.size() == 2) {
            twoSpanCount++;
            // Left prong: [0, 10], Right prong: [20, 30]
            assert(ySpans[0].xStart == 0 && ySpans[0].xEnd == 10);
            assert(ySpans[1].xStart == 20 && ySpans[1].xEnd == 30);
        }
    }
    assert(twoSpanCount == 20);

    std::cout << " -> Concave interior correctly split into disjoint spans adhering to parity rule.\n";
}

void testAsteroidProceduralPolygonFill() {
    std::cout << "\n[TEST 4] Scan-Line Fill: Game Asteroid 10-Vertex Irregular Geometry...\n";

    // Reproduce the exact 10-vertex radial asteroid profile from Renderer::renderAsteroid
    const int NUM_SECTORS = 10;
    const float angles[NUM_SECTORS] = {
        0.0f, 0.628f, 1.256f, 1.884f, 2.512f,
        3.141f, 3.769f, 4.397f, 5.025f, 5.653f
    };
    const float radiusRatios[NUM_SECTORS] = {
        1.0f, 0.82f, 1.08f, 0.90f, 1.15f,
        0.85f, 1.05f, 0.78f, 1.12f, 0.92f
    };

    float cx = 200.0f;
    float cy = 300.0f;
    float radius = 25.0f;

    std::vector<Point2D> asteroidVertices;
    for (int i = 0; i < NUM_SECTORS; ++i) {
        float r = radius * radiusRatios[i];
        float px = cx + r * std::cos(angles[i]);
        float py = cy + r * std::sin(angles[i]);
        asteroidVertices.push_back({ px, py });
    }

    auto spans = ScanLineFiller::calculateScanlineSpans(asteroidVertices);
    assert(!spans.empty());

    // Verify all spans fall inside the theoretical bounding box [cx - maxR, cx + maxR]
    float maxR = radius * 1.15f + 1.0f;
    for (const auto& s : spans) {
        assert(s.y >= static_cast<int>(cy - maxR) && s.y <= static_cast<int>(cy + maxR));
        assert(s.xStart <= s.xEnd);
        assert(s.xStart >= static_cast<int>(cx - maxR) && s.xEnd <= static_cast<int>(cx + maxR));
    }

    auto pixels = ScanLineFiller::calculateFilledPixels(asteroidVertices);
    assert(pixels.size() > 1000); // Substantial filled rock area

    std::cout << " -> 10-vertex irregular asteroid rock rasterized into " << spans.size()
              << " scanlines (" << pixels.size() << " discrete interior pixels).\n";
}

void testHorizontalAndDegenerateEdges() {
    std::cout << "\n[TEST 5] Edge Cases: Horizontal & Slanted Degenerate Edges...\n";

    // Stepped polygon with multiple horizontal edges
    std::vector<Point2D> stepped = {
        { 0.0f, 0.0f },
        { 20.0f, 0.0f },
        { 20.0f, 10.0f },
        { 40.0f, 10.0f },
        { 40.0f, 20.0f },
        { 0.0f, 20.0f }
    };

    auto spans = ScanLineFiller::calculateScanlineSpans(stepped);
    assert(!spans.empty());
    assert(spans.size() == 20); // 20 scanlines from y=0 to 19

    // Check width of lower tier (y=0..9) is [0..20], upper tier (y=10..19) is [0..40]
    for (const auto& s : spans) {
        if (s.y < 10) {
            assert(s.xStart == 0 && s.xEnd == 20);
        } else {
            assert(s.xStart == 0 && s.xEnd == 40);
        }
    }

    std::cout << " -> Stepped polygon with horizontal boundaries filled without division-by-zero.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: CGL CO-3 SCAN-LINE POLYGON FILL SUITE \n";
    std::cout << "=====================================================\n\n";

    testTriangleScanlineFill();
    testConvexRectangleScanlineFill();
    testConcavePolygonOddEvenParity();
    testAsteroidProceduralPolygonFill();
    testHorizontalAndDegenerateEdges();

    std::cout << "\n>>> ALL CGL CO-3 SCAN-LINE POLYGON FILL TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
